Disk On RAM source code

Let’s us create a directory called DiskOnRAM which holds the following six files — three ‘C’ source files, two ‘C’ headers, and one Makefile.
partition.h
*************************************************************	
#ifndef PARTITION_H
#define PARTITION_H
 
#include <linux/types.h>
 
extern void copy_mbr_n_br(u8 *disk);
#endif
******************************************************************
partition.c
******************************************************************

	
#include <linux/string.h>
 
#include "partition.h"
 
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
 
#define SECTOR_SIZE 512
#define MBR_SIZE SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16 // sizeof(PartEntry)
#define PARTITION_TABLE_SIZE 64 // sizeof(PartTable)
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55
#define BR_SIZE SECTOR_SIZE
#define BR_SIGNATURE_OFFSET 510
#define BR_SIGNATURE_SIZE 2
#define BR_SIGNATURE 0xAA55
 
typedef struct
{
    unsigned char boot_type; // 0x00 - Inactive; 0x80 - Active (Bootable)
    unsigned char start_head;
    unsigned char start_sec:6;
    unsigned char start_cyl_hi:2;
    unsigned char start_cyl;
    unsigned char part_type;
    unsigned char end_head;
    unsigned char end_sec:6;
    unsigned char end_cyl_hi:2;
    unsigned char end_cyl;
    unsigned long abs_start_sec;
    unsigned long sec_in_part;
} PartEntry;
 
typedef PartEntry PartTable[4];
 
static PartTable def_part_table =
{
    {
        boot_type: 0x00,
        start_head: 0x00,
        start_sec: 0x2,
        start_cyl: 0x00,
        part_type: 0x83,
        end_head: 0x00,
        end_sec: 0x20,
        end_cyl: 0x09,
        abs_start_sec: 0x00000001,
        sec_in_part: 0x0000013F
    },
    {
        boot_type: 0x00,
        start_head: 0x00,
        start_sec: 0x1,
        start_cyl: 0x0A, // extended partition start cylinder (BR location)
        part_type: 0x05,
        end_head: 0x00,
        end_sec: 0x20,
        end_cyl: 0x13,
        abs_start_sec: 0x00000140,
        sec_in_part: 0x00000140
    },
    {
        boot_type: 0x00,
        start_head: 0x00,
        start_sec: 0x1,
        start_cyl: 0x14,
        part_type: 0x83,
        end_head: 0x00,
        end_sec: 0x20,
        end_cyl: 0x1F,
        abs_start_sec: 0x00000280,
        sec_in_part: 0x00000180
    },
    {
    }
};
static unsigned int def_log_part_br_cyl[] = {0x0A, 0x0E, 0x12};
static const PartTable def_log_part_table[] =
{
    {
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x2,
            start_cyl: 0x0A,
            part_type: 0x83,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x0D,
            abs_start_sec: 0x00000001,
            sec_in_part: 0x0000007F
        },
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x1,
            start_cyl: 0x0E,
            part_type: 0x05,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x11,
            abs_start_sec: 0x00000080,
            sec_in_part: 0x00000080
        },
    },
    {
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x2,
            start_cyl: 0x0E,
            part_type: 0x83,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x11,
            abs_start_sec: 0x00000001,
            sec_in_part: 0x0000007F
        },
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x1,
            start_cyl: 0x12,
            part_type: 0x05,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x13,
            abs_start_sec: 0x00000100,
            sec_in_part: 0x00000040
        },
    },
    {
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x2,
            start_cyl: 0x12,
            part_type: 0x83,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x13,
            abs_start_sec: 0x00000001,
            sec_in_part: 0x0000003F
        },
    }
};
 
static void copy_mbr(u8 *disk)
{
    memset(disk, 0x0, MBR_SIZE);
    *(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
    memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}
static void copy_br(u8 *disk, int start_cylinder, const PartTable *part_table)
{
    disk += (start_cylinder * 32 /* sectors / cyl */ * SECTOR_SIZE);
    memset(disk, 0x0, BR_SIZE);
    memcpy(disk + PARTITION_TABLE_OFFSET, part_table,
        PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + BR_SIGNATURE_OFFSET) = BR_SIGNATURE;
}
void copy_mbr_n_br(u8 *disk)
{
    int i;
 
    copy_mbr(disk);
    for (i = 0; i < ARRAY_SIZE(def_log_part_table); i++)
    {
        copy_br(disk, def_log_part_br_cyl[i], &def_log_part_table[i]);
    }
}

******************************************************************


ram_device.h

******************************************************************
	
#ifndef RAMDEVICE_H
#define RAMDEVICE_H
 
#define RB_SECTOR_SIZE 512
 
extern int ramdevice_init(void);
extern void ramdevice_cleanup(void);
extern void ramdevice_write(sector_t sector_off, u8 *buffer, unsigned int sectors);
extern void ramdevice_read(sector_t sector_off, u8 *buffer, unsigned int sectors);
#endif
ram_device.c

	
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
 
#include "ram_device.h"
#include "partition.h"
 
#define RB_DEVICE_SIZE 1024 /* sectors */
/* So, total device size = 1024 * 512 bytes = 512 KiB */
 
/* Array where the disk stores its data */
static u8 *dev_data;
 
int ramdevice_init(void)
{
    dev_data = vmalloc(RB_DEVICE_SIZE * RB_SECTOR_SIZE);
    if (dev_data == NULL)
        return -ENOMEM;
    /* Setup its partition table */
    copy_mbr_n_br(dev_data);
    return RB_DEVICE_SIZE;
}
 
void ramdevice_cleanup(void)
{
    vfree(dev_data);
}
 
void ramdevice_write(sector_t sector_off, u8 *buffer, unsigned int sectors)
{
    memcpy(dev_data + sector_off * RB_SECTOR_SIZE, buffer,
        sectors * RB_SECTOR_SIZE);
}
void ramdevice_read(sector_t sector_off, u8 *buffer, unsigned int sectors)
{
    memcpy(buffer, dev_data + sector_off * RB_SECTOR_SIZE,
        sectors * RB_SECTOR_SIZE);
}

******************************************************************


ram_block.c
******************************************************************

	
/* Disk on RAM Driver */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/errno.h>
 
#include "ram_device.h"
 
#define RB_FIRST_MINOR 0
#define RB_MINOR_CNT 16
 
static u_int rb_major = 0;
 
/*
 * The internal structure representation of our Device
 */
static struct rb_device
{
    /* Size is the size of the device (in sectors) */
    unsigned int size;
    /* For exclusive access to our request queue */
    spinlock_t lock;
    /* Our request queue */
    struct request_queue *rb_queue;
    /* This is kernel's representation of an individual disk device */
    struct gendisk *rb_disk;
} rb_dev;
 
static int rb_open(struct block_device *bdev, fmode_t mode)
{
    unsigned unit = iminor(bdev->bd_inode);
 
    printk(KERN_INFO "rb: Device is opened\n");
    printk(KERN_INFO "rb: Inode number is %d\n", unit);
 
    if (unit > RB_MINOR_CNT)
        return -ENODEV;
    return 0;
}
 
static int rb_close(struct gendisk *disk, fmode_t mode)
{
    printk(KERN_INFO "rb: Device is closed\n");
    return 0;
}
 
/*
 * Actual Data transfer
 */
static int rb_transfer(struct request *req)
{
    //struct rb_device *dev = (struct rb_device *)(req->rq_disk->private_data);
 
    int dir = rq_data_dir(req);
    sector_t start_sector = blk_rq_pos(req);
    unsigned int sector_cnt = blk_rq_sectors(req);
 
    struct bio_vec *bv;
    struct req_iterator iter;
 
    sector_t sector_offset;
    unsigned int sectors;
    u8 *buffer;
 
    int ret = 0;
 
    //printk(KERN_DEBUG "rb: Dir:%d; Sec:%lld; Cnt:%d\n", dir, start_sector, sector_cnt);
 
    sector_offset = 0;
    rq_for_each_segment(bv, req, iter)
    {
        buffer = page_address(bv->bv_page) + bv->bv_offset;
        if (bv->bv_len % RB_SECTOR_SIZE != 0)
        {
            printk(KERN_ERR "rb: Should never happen: "
                "bio size (%d) is not a multiple of RB_SECTOR_SIZE (%d).\n"
                "This may lead to data truncation.\n",
                bv->bv_len, RB_SECTOR_SIZE);
            ret = -EIO;
        }
        sectors = bv->bv_len / RB_SECTOR_SIZE;
        printk(KERN_DEBUG "rb: Sector Offset: %lld; Buffer: %p; Length: %d sectors\n",
            sector_offset, buffer, sectors);
        if (dir == WRITE) /* Write to the device */
        {
            ramdevice_write(start_sector + sector_offset, buffer, sectors);
        }
        else /* Read from the device */
        {
            ramdevice_read(start_sector + sector_offset, buffer, sectors);
        }
        sector_offset += sectors;
    }
    if (sector_offset != sector_cnt)
    {
        printk(KERN_ERR "rb: bio info doesn't match with the request info");
        ret = -EIO;
    }
 
    return ret;
}
 
/*
 * Represents a block I/O request for us to execute
 */
static void rb_request(struct request_queue *q)
{
    struct request *req;
    int ret;
 
    /* Gets the current request from the dispatch queue */
    while ((req = blk_fetch_request(q)) != NULL)
    {
#if 0
        /*
         * This function tells us whether we are looking at a filesystem request
         * - one that moves block of data
         */
        if (!blk_fs_request(req))
        {
            printk(KERN_NOTICE "rb: Skip non-fs request\n");
            /* We pass 0 to indicate that we successfully completed the request */
            __blk_end_request_all(req, 0);
            //__blk_end_request(req, 0, blk_rq_bytes(req));
            continue;
        }
#endif
        ret = rb_transfer(req);
        __blk_end_request_all(req, ret);
        //__blk_end_request(req, ret, blk_rq_bytes(req));
    }
}
 
/*
 * These are the file operations that performed on the ram block device
 */
static struct block_device_operations rb_fops =
{
    .owner = THIS_MODULE,
    .open = rb_open,
    .release = rb_close,
};
 
/*
 * This is the registration and initialization section of the ram block device
 * driver
 */
static int __init rb_init(void)
{
    int ret;
 
    /* Set up our RAM Device */
    if ((ret = ramdevice_init()) < 0)
    {
        return ret;
    }
    rb_dev.size = ret;
 
    /* Get Registered */
    rb_major = register_blkdev(rb_major, "rb");
    if (rb_major <= 0)
    {
        printk(KERN_ERR "rb: Unable to get Major Number\n");
        ramdevice_cleanup();
        return -EBUSY;
    }
 
    /* Get a request queue (here queue is created) */
    spin_lock_init(&rb_dev.lock);
    rb_dev.rb_queue = blk_init_queue(rb_request, &rb_dev.lock);
    if (rb_dev.rb_queue == NULL)
    {
        printk(KERN_ERR "rb: blk_init_queue failure\n");
        unregister_blkdev(rb_major, "rb");
        ramdevice_cleanup();
        return -ENOMEM;
    }
 
    /*
     * Add the gendisk structure
     * By using this memory allocation is involved,
     * the minor number we need to pass bcz the device
     * will support this much partitions
     */
    rb_dev.rb_disk = alloc_disk(RB_MINOR_CNT);
    if (!rb_dev.rb_disk)
    {
        printk(KERN_ERR "rb: alloc_disk failure\n");
        blk_cleanup_queue(rb_dev.rb_queue);
        unregister_blkdev(rb_major, "rb");
        ramdevice_cleanup();
        return -ENOMEM;
    }
 
    /* Setting the major number */
    rb_dev.rb_disk->major = rb_major;
    /* Setting the first mior number */
    rb_dev.rb_disk->first_minor = RB_FIRST_MINOR;
    /* Initializing the device operations */
    rb_dev.rb_disk->fops = &rb_fops;
    /* Driver-specific own internal data */
    rb_dev.rb_disk->private_data = &rb_dev;
    rb_dev.rb_disk->queue = rb_dev.rb_queue;
    /*
     * You do not want partition information to show up in
     * cat /proc/partitions set this flags
     */
    //rb_dev.rb_disk->flags = GENHD_FL_SUPPRESS_PARTITION_INFO;
    sprintf(rb_dev.rb_disk->disk_name, "rb");
    /* Setting the capacity of the device in its gendisk structure */
    set_capacity(rb_dev.rb_disk, rb_dev.size);
 
    /* Adding the disk to the system */
    add_disk(rb_dev.rb_disk);
    /* Now the disk is "live" */
    printk(KERN_INFO "rb: Ram Block driver initialised (%d sectors; %d bytes)\n",
        rb_dev.size, rb_dev.size * RB_SECTOR_SIZE);
 
    return 0;
}
/*
 * This is the unregistration and uninitialization section of the ram block
 * device driver
 */
static void __exit rb_cleanup(void)
{
    del_gendisk(rb_dev.rb_disk);
    put_disk(rb_dev.rb_disk);
    blk_cleanup_queue(rb_dev.rb_queue);
    unregister_blkdev(rb_major, "rb");
    ramdevice_cleanup();
}
 
module_init(rb_init);
module_exit(rb_cleanup);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia <email@sarika-pugs.com>");
MODULE_DESCRIPTION("Ram Block Driver");
MODULE_ALIAS_BLOCKDEV_MAJOR(rb_major);

You can also download the code demonstrated from here.

As usual, executing make will build the ‘Disk on RAM’ driver (dor.ko), combining the three ‘C’ files. Check out the Makefile to see how.
Makefile
******************************************************************

	
# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)
 
    KERNEL_SOURCE := /usr/src/linux
    PWD := $(shell pwd)
default: module
 
module:
    $(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules
 
clean:
    $(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
 
# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else
 
    obj-m := dor.o
    dor-y := ram_block.o ram_device.o partition.o
 
endif


******************************************************************

To clean the built files, run the usual make clean.

Once built, the following are the experimental steps (refer to Figures 1 to 3).

Playing with the 'Disk on RAM' driver
    Figure 1: Playing with the ‘Disk on RAM’ driver

xxd showing the initial data on the first partition (/dev/rb1)
    Figure 2: xxd showing the initial data on the first partition (/dev/rb1)

Formatting the third partition (/dev/rb3)
    Figure 3: Formatting the third partition (/dev/rb3)


Please note that all these need to be executed with root privileges:

    Load the driver dor.ko using insmod. This would create the block device files representing the disk on 512 KiB of RAM, with three primary and three logical partitions.
    Check out the automatically created block device files (/dev/rb*). /dev/rb is the entire disk, which is 512 KiB in size. rb1, rb2 and rb3 are the primary partitions, with rb2 being the extended partition and containing three logical partitions rb5, rb6 and rb7.
    Read the entire disk (/dev/rb) using the disk dump utility dd.
    Zero out the first sector of the disk’s first partition (/dev/rb1), again using dd.
    Write some text into the disk’s first partition (/dev/rb1) using cat.
    Display the initial contents of the first partition (/dev/rb1) using the xxd utility. See Figure 2 for xxd output.
    Display the partition information for the disk using fdisk. See Figure 3 for fdisk output.
    Quick-format the third primary partition (/dev/rb3) as a vfat filesystem (like your pen drive), using mkfs.vfat (Figure 3).
    Mount the newly formatted partition using mount, say at /mnt (Figure 3).
    The disk usage utility df would now show this partition mounted at /mnt (Figure 3). You may go ahead and store files there, but remember that this is a disk on RAM, and so is non-persistent.
    Unload the driver using rmmod dor after unmounting the partition using umount /mnt. All data on the disk will be lost.

Now let’s learn the rules

We have just played around with the disk on RAM (DOR), but without actually knowing the rules, i.e., the internal details of the game. So, let’s dig into the nitty-gritty to decode the rules. Each of the three .c files represent a specific part of the driver; ram_device.c and ram_device.h abstract the underlying RAM operations like vmalloc/vfree, memcpy, etc., providing disk operation APIs like init/cleanup, read/write, etc.

partition.c and partition.h provide the functionality to emulate the various partition tables on the DOR. Recall the pre-lunch session (i.e., the previous article) to understand the details of partitioning.

The code in this is responsible for the partition information like the number, type, size, etc., that is shown using fdisk. The ram_block.c file is the core block driver implementation, exposing the DOR as the block device files (/dev/rb*) to user-space. In other words, four of the five files ram_device.* and partition.* form the horizontal layer of the device driver, and ram_block.c forms the vertical (block) layer of the device driver. So, let’s understand that in detail.
The block driver basics

Conceptually, the block drivers are very similar to character drivers, especially with regards to the following:

    Usage of device files
    Major and minor numbers
    Device file operations
    Concept of device registration

So, if you already know character driver implementation, it would be easy to understand block drivers.

However, they are definitely not identical. The key differences are as follows:

    Abstraction for block-oriented versus byte-oriented devices.
    Block drivers are designed to be used by I/O schedulers, for optimal performance. Compare that with character drivers that are to be used by VFS.
    Block drivers are designed to be integrated with the Linux buffer cache mechanism for efficient data access. Character drivers are pass-through drivers, accessing the hardware directly.

And these cause the implementation differences. Let’s analyse the key code snippets from ram_block.c, starting at the driver’s constructor rb_init().

The first step is to register for an 8-bit (block) major number (which implicitly means registering for all 256 8-bit minor numbers associated with it). The function for that is as follows:
int register_blkdev(unsigned int major, const char *name);

Here, major is the major number to be registered, and name is a registration label displayed under the kernel window /proc/devices. Interestingly, register_blkdev() tries to allocate and register a freely available major number, when 0 is passed for its first parameter major; on success, the allocated major number is returned. The corresponding de-registration function is as follows:
void unregister_blkdev(unsigned int major, const char *name);

Both these are prototyped in <linux/fs.h>.

The second step is to provide the device file operations, through the struct block_device_operations (prototyped in <linux/blkdev.h>) for the registered major number device files.

However, these operations are too few compared to the character device file operations, and mostly insignificant. To elaborate, there are no operations even to read and write, which is surprising. But as we already know that block drivers need to integrate with the I/O schedulers, the read-write implementation is achieved through something called request queues. So, along with providing the device file operations, the following need to be provided:

    The request queue for queuing the read/write requests
    The spin lock associated with the request queue to protect its concurrent access
    The request function to process the requests in the request queue

Also, there is no separate interface for block device file creations, so the following are also provided:

    The device file name prefix, commonly referred to as disk_name (rb in the dor driver)
    The starting minor number for the device files, commonly referred to as first_minor.

Finally, two block-device-specific things are also provided, namely:

    The maximum number of partitions supported for this block device, by specifying the total minors.
    The underlying device size in units of 512-byte sectors, for the logical block access abstraction.

All these are registered through the struct gendisk using the following function:
void add_disk(struct gendisk *disk);

The corresponding delete function is as follows:
void del_gendisk(struct gendisk *disk);

Prior to add_disk(), the various fields of struct gendisk need to initialised, either directly or using various macros/functions like set_capacity(). major, first_minor, fops, queue, disk_name are the minimal fields to be initialised directly. And even before the initialisation of these fields, the struct gendisk needs to be allocated, using the function given below:
struct gendisk *alloc_disk(int minors);

Here, minors is the total number of partitions supported for this disk. And the corresponding inverse function would be:
void put_disk(struct gendisk *disk);

All these are prototyped in <linux/genhd.h>.
Request queue and the request function

The request queue also needs to be initialised and set up into the struct gendisk, before add_disk(). The request queue is initialised by calling:
struct request_queue *blk_init_queue(request_fn_proc *, spinlock_t *);

We provide the request-processing function and the initialised concurrency protection spin-lock as parameters. The corresponding queue clean-up function is given below:
void blk_cleanup_queue(struct request_queue *);

The request (processing) function should be defined with the following prototype:
void request_fn(struct request_queue *q);

It should be coded to fetch a request from its parameter q, for instance, by using the following:
struct request *blk_fetch_request(struct request_queue *q);

Then it should either process it, or initiate processing. Whatever it does should be non-blocking, as this request function is called from a non-process context, and also after taking the queue’s spin-lock. Moreover, only functions not releasing or taking the queue’s spin-lock should be used within the request function.

A typical example of request processing, as demonstrated by the function rb_request() in ram_block.c is given below:
while ((req = blk_fetch_request(q)) != NULL) /* Fetching a request */
{
        /* Processing the request: the actual data transfer */
        ret = rb_transfer(req); /* Our custom function */
         /* Informing that the request has been processed with return of ret */
        __blk_end_request_all(req, ret);
}
Requests and their processing

Our key function is rb_transfer(), which parses a struct request and accordingly does the actual data transfer. The struct request primarily contains the direction of data transfer, the starting sector for the data transfer, the total number of sectors for the data transfer, and the scatter-gather buffer for the data transfer. The various macros to extract these from the struct request are as follows:
rq_data_dir(req); /* Operation type: 0 - read from device; otherwise - write to device */
blk_req_pos(req); /* Starting sector to process */
blk_req_sectors(req); /* Total sectors to process */
rq_for_each_segment(bv, req, iter) /* Iterator to extract individual buffers */

rq_for_each_segment() is the special one which iterates over the struct request (req) using iter, and extracting the individual buffer information into the struct bio_vec (bv: basic input/output vector) on each iteration. And then, on each extraction, the appropriate data transfer is done, based on the operation type, invoking one of the following APIs from ram_device.c:
void ramdevice_write(sector_t sector_off, u8 *buffer, unsigned int sectors);
void ramdevice_read(sector_t sector_off, u8 *buffer, unsigned int sectors);

Check out the complete code of rb_transfer() in ram_block.c.
Summing up

With that, we have actually learnt the beautiful block drivers by traversing through the design of a hard disk and playing around with partitioning, formatting and various other raw operations on a hard disk. Thanks for patiently listening. Now, the session is open for questions — please feel free to leave your queries as comments.
