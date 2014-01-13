/*
 * module5.h
 *
 * Public constants and other details to allow user level applications
 * to communicate with module5 via ioctl operations
 */

#ifndef __MODULE5_H__

// use 0xC0 as an unused magic number (see documentation link above)

#define MODULE5_MAGIC       0xC0

// we'll define four commands for our module:
// 0 - a generalized reset command (will turn off LEDs)
// 1 - a write command to output raw bit pattern value
// 2 - a write command to output a glyph based on value
// 3 - a read command to retrieve last known value written to LEDs

#define MODULE5_IOCRESET    _IO(MODULE5_MAGIC, 0)
#define MODULE5_IOCSPATTERN _IOW(MODULE5_MAGIC, 1, int)
#define MODULE5_IOCSGLYPH   _IOW(MODULE5_MAGIC, 2, int)
#define MODULE5_IOCGVALUE   _IOR(MODULE5_MAGIC, 3, int)

#define MODULE5_MAXCOMMANDS 4

#define __MODULE5_H__
#endif

