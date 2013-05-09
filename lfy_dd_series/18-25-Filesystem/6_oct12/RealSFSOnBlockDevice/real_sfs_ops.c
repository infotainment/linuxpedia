#include <linux/fs.h> /* For struct super_block */
#include <linux/errno.h> /* For error codes */
#include <linux/buffer_head.h> /* struct buffer_head, sb_bread, ... */
#include <linux/string.h> /* For memcpy */
#include <linux/vmalloc.h> /* For vmalloc, ... */

#include "real_sfs_ds.h"
#include "real_sfs_ops.h"

static int read_sb_from_real_sfs(sfs_info_t *info, sfs_super_block_t *sb)
{
	struct buffer_head *bh;

	if (!(bh = sb_bread(info->vfs_sb, 0 /* Super block is the 0th block */)))
	{
		return -EIO;
	}
	memcpy(sb, bh->b_data, SIMULA_FS_BLOCK_SIZE);
	brelse(bh);
	return 0;
}
static int read_from_real_sfs(sfs_info_t *info, byte4_t block, byte4_t offset, void *buf, byte4_t len)
{
	byte4_t block_size = info->sb.block_size;
	struct buffer_head *bh;

	if (offset >= block_size)
	{
		block += (offset / block_size);
		offset %= block_size;
	}
	if (offset + len > block_size) // Should never happen
	{
		return -EINVAL;
	}
	if (!(bh = sb_bread(info->vfs_sb, block)))
	{
		return -EIO;
	}
	memcpy(buf, bh->b_data + offset, len);
	brelse(bh);
	return 0;
}

int init_browsing(sfs_info_t *info)
{
	byte1_t *used_blocks;
	int i, j;
	sfs_file_entry_t fe;

	if (read_sb_from_real_sfs(info, &info->sb) < 0)
	{
		return -EIO;
	}
	if (info->sb.type != SIMULA_FS_TYPE)
	{
		printk(KERN_ERR "Invalid SFS detected. Giving up.\n");
		return -EINVAL;
	}

	/* Mark used blocks */
	used_blocks = (byte1_t *)(vmalloc(info->sb.partition_size));
	if (!used_blocks)
	{
		return -ENOMEM;
	}
	for (i = 0; i < info->sb.data_block_start; i++)
	{
		used_blocks[i] = 1;
	}
	for (; i < info->sb.partition_size; i++)
	{
		used_blocks[i] = 0;
	}

	for (i = 0; i < info->sb.entry_count; i++)
	{
		if (read_from_real_sfs(info, info->sb.entry_table_block_start, i * sizeof(sfs_file_entry_t), &fe, sizeof(sfs_file_entry_t)) < 0)
		{
			vfree(used_blocks);
			return -ENOMEM;
		}
		if (!fe.name[0]) continue;
		for (j = 0; j < SIMULA_FS_DATA_BLOCK_CNT; j++)
		{
			if (fe.blocks[j] == 0) break;
			used_blocks[fe.blocks[j]] = 1;
		}
	}

	info->used_blocks = used_blocks;
	info->vfs_sb->s_fs_info = info;
	return 0;
}
void shut_browsing(sfs_info_t *info)
{
	if (info->used_blocks)
		vfree(info->used_blocks);
}
