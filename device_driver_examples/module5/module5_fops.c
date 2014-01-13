/*
 * module5_fops.c
 *
 * This is a fifth cut at a Linux driver module. This
 * module will announce its init/cleanup, and support
 * a number of file operations that actually communicate
 * with real hardware (parallel port). We will attempt to
 * light up a 7 segment LED on a parallel cable harness, 
 * based on data written to this driver.
 *
 * This file specifically implements the file operations
 * for module5.
 */


/*
 * special header files for Linux modules
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/stat.h>

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include "module5.h"
#include "module5_priv.h"




/*
 * implementation of struct file_operations support functions
 */





/*
 * int module5_open (struct inode *i, struct file *fp);
 *
 * This function is called when a user level application attempts to
 * open the device file (/dev/module5 for example). This will
 * initialize our "pretend data source" of an array that holds some
 * formatted data (a counte of the number of times we've successfully opened
 * the module). 
 */

int
module5_open (struct inode *i, struct file *fp)
{
	printk (KERN_INFO "module5: module5_open starting ... \n"); 

	// we will limit out char driver to only allow one
	// user app to connect at one time. why? does it make
	// sense for two or more apps to try to read from some sort
	// of hardware simultaneously? Think of a serial port. If two
	// apps attempt to read, how would the stream of bytes being
	// received or sent be distributed to two applications??
	// Hence, return an error code (EBUSY in this case)

	if (module5_flagopen) {
		return -EBUSY;
	}	/* endif */

	module5_flagopen++;

	// this driver will support write operations from the user application,
	// and store data written into our module global variable
	// module5_lastbytewritten ... as we won't support a "true read" from
	// parallel port hardware

	module5_lastbytewritten = 0;

	// all is well!

	return 0;
}	/* end module5_open */


/*
 * int module5_ioctl (struct inode *i, struct file *fp, unsigned int cmd, unsigned long arg);
 *
 * This function will support input/output command processing for
 * this module.
 */

int
module5_ioctl (struct inode *i, struct file *fp, unsigned int cmd, unsigned long arg)
{
	int val;
	int err;
	char glyphs[16] = { 
		// insert appropriate patterns for hex digits
	};

	// ensure we've got correct magic number and command makes sense

	if (_IOC_TYPE(cmd) != MODULE5_MAGIC)
		return -ENOTTY;

	if (_IOC_NR(cmd) >= MODULE5_MAXCOMMANDS)
		return -ENOTTY;

	// verify that our read/write operations are possible. Note
	// that when user level wants to read, we have to verify that
	// the kernel has WRITE ACCESS to the supplied user level pointer,
	// and that if the user app wants to write, that the kernel has
	// READ ACCESS to the suplied user level pointer. It seems
	// odd to do this, but we're validating the pointer supplied as
	// an argument here! If there's a failure, return -EFAULT

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok (VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok (VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	
	if (err)
		return -EFAULT;

	// note that if you decide not to do the above access checking,
	// you can always use put_user() and get_user() as alternatives,
	// as obth of those will return non-zero to indicate invalid
	// access to user space. If you do implement the above, then
	// you can use __put_user() and __get_user() as faster ways
	// of accessing hardware.

	switch (cmd) {
	case MODULE5_IOCRESET:
		// turn off all LED segments
		outb (0x80, MODULE5_PORT);
		break;

	case MODULE5_IOCSPATTERN:
		// get pattern from user memory, and output

		__get_user (val, (int __user *)arg);
		outb (val | 0x80, MODULE5_PORT);
		break;

	case MODULE5_IOCSGLYPH:
		// get glyph value from user memory, and output
		// matching bit pattern

		__get_user (val, (int __user *)arg);
		val &= 0x0F;	// use only low 4 bits as index into glyph table
		outb (glyphs[val] | 0x80, MODULE5_PORT);
		break;

	case MODULE5_IOCGVALUE:
		// give the last value written back to user space
		val = module5_lastbytewritten;
		__put_user (val, (int __user *)arg);
		break;

	default:
		return -ENOTTY;
	}	/* end switch */

	return 0;
}	/* end module5_ioctl */


/*
 * int module5_release (struct inode *i, struct file *fp);
 *
 * This function provides support to release access to the module
 * and to increment the usage flag to allow another app to gain
 * access to the module at user level.
 */

int
module5_release (struct inode *i, struct file *fp)
{
	// indicate that future calls to open() will succeed

	module5_flagopen--;

	printk (KERN_INFO "module5: module5_release ... \n"); 

	return 0;
}	/* end module5_release */


