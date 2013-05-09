#ifndef REAL_SFS_OPS_H
#define REAL_SFS_OPS_H

#include <linux/fs.h>

#include "real_sfs_ds.h"

#define ROOT_INODE_NUM 0
#define S2V_INODE_NUM(i) ((i) + 1) // SFS to VFS
#define V2S_INODE_NUM(i) ((i) - 1) // VFS to SFS

int init_browsing(sfs_info_t *info);
void shut_browsing(sfs_info_t *info);
int sfs_get_data_block(sfs_info_t *info);
void sfs_put_data_block(sfs_info_t *info, int i);
int sfs_list(sfs_info_t *info, struct file *file, void *dirent, filldir_t filldir);
int sfs_create(sfs_info_t *info, char *fn, int perms, sfs_file_entry_t *fe);
int sfs_lookup(sfs_info_t *info, char *fn, sfs_file_entry_t *fe);
int sfs_remove(sfs_info_t *info, char *fn);
int sfs_get_file_entry(sfs_info_t *info, int vfs_ino, sfs_file_entry_t *fe);
int sfs_update(sfs_info_t *info, int vfs_ino, int *size, int *timestamp, int *perms);
int sfs_update_file_entry(sfs_info_t *info, int vfs_ino, sfs_file_entry_t *fe);

#endif
