#ifndef SFS_DS_H
#define SFS_DS_H

#define SIMULA_FS_TYPE 0x13090D15 /* Magic Number for our file system */
#define SIMULA_FS_BLOCK_SIZE 512 /* in bytes */
#define SIMULA_FS_ENTRY_SIZE 64 /* in bytes */
#define SIMULA_FS_DATA_BLOCK_CNT ((SIMULA_FS_ENTRY_SIZE - (16 + 3 * 4)) / 4)

typedef unsigned char uint1_t;
typedef unsigned int uint4_t;
typedef unsigned long long uint8_t;

typedef struct sfs_super_block
{
	uint4_t type; /* Magic number to identify the file system */
	uint4_t block_size; /* Unit of allocation */
	uint4_t partition_size; /* in blocks */
	uint4_t entry_size; /* in bytes */
	uint4_t entry_table_size; /* in blocks */
	uint4_t entry_table_block_start; /* in blocks */
	uint4_t entry_count; /* Total entries in the file system */
	uint4_t data_block_start; /* in blocks */
	uint4_t reserved[SIMULA_FS_BLOCK_SIZE / 4 - 8];
} sfs_super_block_t; /* Making it of SIMULA_FS_BLOCK_SIZE */

typedef struct sfs_file_entry
{
	char name[16];
	uint4_t size; /* in bytes */
	uint4_t timestamp; /* Seconds since Epoch */
	uint4_t perms; /* Permissions for user */
	uint4_t blocks[SIMULA_FS_DATA_BLOCK_CNT];
} sfs_file_entry_t;

#endif
