/*
 * module4.c
 *
 * This is a fourth cut at a Linux driver module. This
 * module will announce its init/cleanup, and support
 * a number of file operations that actually communicate
 * with real hardware (parallel port). We will attempt to
 * light up an LED on a parallel cable harness, 
 * based on data written to this driver.
 *
 * Remember, view logging output via
 * a commandline such as:
 *
 * tail /var/log/messages
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


/*
 * the port we will control ...
 * PC parallel port is 378h by default
 */

#define MODULE4_PORT 		0x378


/*
 * announce our software license and authorship
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Module 4");

/*
 * module device number support
 */

static char *module4_name = "module4";
static int module4_major = 0;
static int module4_minor = 5;
static dev_t module4_devicenumber;

/*
 * character device support structures
 */

struct cdev module4_cdev;


/*
 * prototypes for struct file_operations handlers
 * and initialization of struct file_operations structure
 * including support to deal with auto increment/decrement of
 * reference/usage count (owner member)
 */

int module4_open (struct inode *i, struct file *fp);
int module4_read (struct file *fp, char __user *buffer, size_t len, loff_t *offset);
int module4_write (struct file *fp, const char __user *buffer, size_t len, loff_t *offset);
int module4_release (struct inode *i, struct file *fp);

static struct file_operations module4_fops = {
    .owner = THIS_MODULE,
    .open = module4_open,
    .read = module4_read,
    .write = module4_write,
    .release = module4_release
};

/*
 * module housekeeping and other module globals
 */

struct resource *module4_region = NULL;
int module4_flagopen = 0;
char module4_lastbytewritten;



/*
 * static int __init initialize_module4(void);
 *
 * This function will support the module initialization.
 * Note the use of the __init modifier for this function.
 */

static int __init
initialize_module4(void)
{
	int rc;

	printk(KERN_INFO "module4: initialize_module4() called.\n");

	// allocate a major number for our module

	rc = alloc_chrdev_region (&module4_devicenumber, module4_minor, 1, module4_name);
	if (rc < 0) {
		printk(KERN_INFO "module4: unable to allocate device region.\n");
		goto init_error_0;
	}	/* endif */

	module4_major = MAJOR (module4_devicenumber);
	printk (KERN_INFO "module4: during INIT ... major: %d minor: %d\n", 
		module4_major, module4_minor);

	// initialize and register our character driver's file operations

	cdev_init (&module4_cdev, &module4_fops);
	module4_cdev.owner = THIS_MODULE;
	module4_cdev.ops = &module4_fops;
	rc = cdev_add (&module4_cdev, module4_devicenumber, 1);
	if (rc) {
		printk(KERN_INFO "module4: unable to add cdev struct.\n");
		goto init_error_1;
	}	/* endif */
		


	// now, initialize our access to I/O Port hardware
	// for this example, we're going to try to connect to
	// parallel port

#ifdef THIS_WAS_THE_OLD_WAY_OF_DOING_THIS
	rc = check_region (MODULE4_PORT, 1);
	if (rc) {
		printk (KERN_INFO "module4: unable to gain access to parallel port ... "
			"but we'll use it anyways!\n");

		// uncomment the next 2 lines if you wish to exit with an error condition
		// rc = -EIO;
		// goto init_error_2;
	}	/* endif */
#endif

	module4_region = request_region (MODULE4_PORT, 1, module4_name);
	if (module4_region == NULL) {
		printk (KERN_INFO "module4: unable to gain exclusive access to parallel port ... "
			"but we'll use it anyways!\n");

		// uncomment the next 2 lines if you wish to exit with an error condition
		// rc = -EIO;
		// goto init_error_2;
	}	/* endif */

	printk (KERN_INFO "module4: successfully completed init!\n");

	return 0;

	// common error support for initialization

init_error_2:
	cdev_del (&module4_cdev);

init_error_1:
	unregister_chrdev_region (module4_devicenumber, 1);

init_error_0:
	return rc;
}	/* end initialize_module4 */


/*
 * static int __exit cleanup_module4(void);
 *
 * This function will support the module cleanup.
 * Note the use of the __exit modifier for this function.
 */

static void __exit
cleanup_module4(void)
{
	// release our 1 device

	printk (KERN_INFO "module4: during EXIT ... major: %d minor: %d\n", 
		module4_major, module4_minor);

	// only deallocate module region IF we had exclusive use

	if (module4_region != NULL)
		release_region (MODULE4_PORT, 1);

	// deallocate other module resources and device numbers

	cdev_del (&module4_cdev);
	unregister_chrdev_region (module4_devicenumber, 1);

	printk(KERN_INFO "module4: cleanup_module4() called.\n");
}	/* end cleanup_module4 */


/*
 * these macros register the appropriate init and cleanup
 * functions that the Linux Kernel expects!
 */

module_init(initialize_module4);
module_exit(cleanup_module4);



/*
 * implementation of struct file_operations support functions
 */





/*
 * int module4_open (struct inode *i, struct file *fp);
 *
 * This function is called when a user level application attempts to
 * open the device file (/dev/module4 for example). This will
 * initialize our "pretend data source" of an array that holds some
 * formatted data (a counte of the number of times we've successfully opened
 * the module). It'll set up a global pointer to this data, so that calls to
 * the module4_read() function can properly manage the data stream available.
 */

int
module4_open (struct inode *i, struct file *fp)
{
	printk (KERN_INFO "module4: module4_open starting ... \n"); 

	// we will limit out char driver to only allow one
	// user app to connect at one time. why? does it make
	// sense for two or more apps to try to read from some sort
	// of hardware simultaneously? Think of a serial port. If two
	// apps attempt to read, how would the stream of bytes being
	// received or sent be distributed to two applications??
	// Hence, return an error code (EBUSY in this case)

	if (module4_flagopen) {
		return -EBUSY;
	}	/* endif */

	module4_flagopen++;

	// this driver will support write operations from the user application,
	// and store data written into our module global variable
	// module4_lastbytewritten ... as we won't support a "true read" from
	// parallel port hardware

	module4_lastbytewritten = 0;

	// all is well!

	return 0;
}	/* end module4_open */


/*
 * int module4_read (struct file *fp, char __user *buffer, size_t len, 
 *     loff_t *offset);
 *
 * This function implements a basic read() handler for this module.
 * It will simulate reading from a device by pulling data out of a single
 * character buffer keeping track of last value written to parallel port hardware
 */

int
module4_read (struct file *fp, char __user *buffer, size_t len, loff_t *offset)
{
	int bytes_read = 0;

	printk (KERN_INFO "module4: module4_read ... \n"); 

	// only provide LAST BYTE written into driver

	copy_to_user (buffer, &module4_lastbytewritten, 1);
	bytes_read = 1;

	return bytes_read;
}	/* end module4_read */


/*
 * int module4_write (struct file *fp, const char __user *buffer, size_t len, 
 *     loff_t *offset);
 *
 * This function implements a basic write() handler for this module.
 * It will physically write to the parallel port of the PC, and buffer
 * the value written for read() support (above)
 */

int
module4_write (struct file *fp, const char __user *buffer, size_t len, loff_t *offset)
{
	int bytes_written = 0;
	char ch;
	const char *tmp;

	printk (KERN_INFO "module4: module4_write ... \n"); 

	tmp = buffer + len - 1;
	copy_from_user (&ch, tmp, 1);	
	bytes_written = 1;

	outb (ch, MODULE4_PORT);

	module4_lastbytewritten = ch;

	return bytes_written;
}	/* end module4_write */


/*
 * int module4_release (struct inode *i, struct file *fp);
 *
 * This function provides support to release access to the module
 * and to increment the usage flag to allow another app to gain
 * access to the module at user level.
 */

int
module4_release (struct inode *i, struct file *fp)
{
	// indicate that future calls to open() will succeed

	module4_flagopen--;

	printk (KERN_INFO "module4: module4_release ... \n"); 

	return 0;
}	/* end module4_release */


