/*
 * module5_init.c
 *
 * This is a fifth cut at a Linux driver module. This
 * module will announce its init/cleanup, and support
 * a number of file operations that actually communicate
 * with real hardware (parallel port). We will attempt to
 * light up a 7 segment LED on a parallel cable harness, 
 * based on data written to this driver.
 *
 * Remember, view logging output via
 * a commandline such as:
 *
 * tail /var/log/messages
 *
 * This file specifically implements startup/cleanup code for module5
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
 * announce our software license and authorship
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Module 4");

/*
 * module device number support
 */

static char *module5_name = "module5";
static int module5_major = 0;
static int module5_minor = 0;
static dev_t module5_devicenumber;

/*
 * character device support structures
 */

struct cdev module5_cdev;


/*
 * prototypes for struct file_operations handlers
 * and initialization of struct file_operations structure
 * including support to deal with auto increment/decrement of
 * reference/usage count (owner member)
 */

int module5_open (struct inode *i, struct file *fp);
int module5_ioctl (struct inode *i, struct file *fp, unsigned int cmd, unsigned long arg);
int module5_release (struct inode *i, struct file *fp);

static struct file_operations module5_fops = {
    .owner = THIS_MODULE,
    .open = module5_open,
    .ioctl = module5_ioctl,
    .release = module5_release
};

/*
 * module housekeeping and other module globals
 */

struct resource *module5_region = NULL;
int module5_flagopen = 0;
char module5_lastbytewritten;



/*
 * static int __init initialize_module5(void);
 *
 * This function will support the module initialization.
 * Note the use of the __init modifier for this function.
 */

static int __init
initialize_module5(void)
{
	int rc;

	printk(KERN_INFO "module5: initialize_module5() called.\n");

	// allocate a major number for our module

	rc = alloc_chrdev_region (&module5_devicenumber, module5_minor, 1, module5_name);
	if (rc < 0) {
		printk(KERN_INFO "module5: unable to allocate device region.\n");
		goto init_error_0;
	}	/* endif */

	module5_major = MAJOR (module5_devicenumber);
	printk (KERN_INFO "module5: during INIT ... major: %d minor: %d\n", 
		module5_major, module5_minor);

	// initialize and register our character driver's file operations

	cdev_init (&module5_cdev, &module5_fops);
	module5_cdev.owner = THIS_MODULE;
	module5_cdev.ops = &module5_fops;
	rc = cdev_add (&module5_cdev, module5_devicenumber, 1);
	if (rc) {
		printk(KERN_INFO "module5: unable to add cdev struct.\n");
		goto init_error_1;
	}	/* endif */
		


	// now, initialize our access to I/O Port hardware
	// for this example, we're going to try to connect to
	// parallel port

#ifdef THIS_WAS_THE_OLD_WAY_OF_DOING_THIS
	rc = check_region (MODULE5_PORT, 1);
	if (rc) {
		printk (KERN_INFO "module5: unable to gain access to parallel port ... "
			"but we'll use it anyways!\n");

		// uncomment the next 2 lines if you wish to exit with an error condition
		// rc = -EIO;
		// goto init_error_2;
	}	/* endif */
#endif

	module5_region = request_region (MODULE5_PORT, 1, module5_name);
	if (module5_region == NULL) {
		printk (KERN_INFO "module5: unable to gain exclusive access to parallel port ... "
			"but we'll use it anyways!\n");

		// uncomment the next 2 lines if you wish to exit with an error condition
		// rc = -EIO;
		// goto init_error_2;
	}	/* endif */

	printk (KERN_INFO "module5: successfully completed init!\n");

	return 0;

	// common error support for initialization

//init_error_2:
//	cdev_del (&module5_cdev);

init_error_1:
	unregister_chrdev_region (module5_devicenumber, 1);

init_error_0:
	return rc;
}	/* end initialize_module5 */


/*
 * static int __exit cleanup_module5(void);
 *
 * This function will support the module cleanup.
 * Note the use of the __exit modifier for this function.
 */

static void __exit
cleanup_module5(void)
{
	// release our 1 device

	printk (KERN_INFO "module5: during EXIT ... major: %d minor: %d\n", 
		module5_major, module5_minor);

	// only deallocate module region IF we had exclusive use

	if (module5_region != NULL)
		release_region (MODULE5_PORT, 1);

	// deallocate other module resources and device numbers

	cdev_del (&module5_cdev);
	unregister_chrdev_region (module5_devicenumber, 1);

	printk(KERN_INFO "module5: cleanup_module5() called.\n");
}	/* end cleanup_module5 */


/*
 * these macros register the appropriate init and cleanup
 * functions that the Linux Kernel expects!
 */

module_init(initialize_module5);
module_exit(cleanup_module5);


