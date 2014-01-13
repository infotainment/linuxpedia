
/*
 * module5_priv.h
 *
 * This file contains private module declarations.
 */

#ifndef __MODULE5_PRIV_H__

/*
 * the port we will control ...
 * PC parallel port is 378h by default
 */

#define MODULE5_PORT 		0x378


/*
 * module housekeeping and other module globals
 */

extern struct resource *module5_region;
extern int module5_flagopen;
extern char module5_lastbytewritten;


extern int module5_open (struct inode *i, struct file *fp);
extern int module5_ioctl (struct inode *i, struct file *fp, unsigned int cmd, unsigned long arg);
extern int module5_release (struct inode *i, struct file *fp);

#define __MODULE5_PRIV_H__
#endif
