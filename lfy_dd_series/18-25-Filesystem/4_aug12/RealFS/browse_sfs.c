#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "sfs_ds.h"

sfs_super_block_t sb;
uint1_t *used_blocks;
uint1_t block[SIMULA_FS_BLOCK_SIZE];

void init_browsing(int sfs_handle)
{
	int i, j;
	sfs_file_entry_t fe;

	/* Mark used blocks */
	used_blocks = (uint1_t *)(calloc(sb.partition_size, sizeof(uint1_t)));
	if (!used_blocks)
	{
		printf("Out of memory\n");
		exit(1);
	}
	for (i = 0; i < sb.data_block_start; i++)
	{
		used_blocks[i] = 1;
	}
	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size, SEEK_SET);
	for (i = 0; i < sb.entry_count; i++)
	{
		read(sfs_handle, &fe, sizeof(sfs_file_entry_t));
		if (!fe.name[0]) continue;
		for (j = 0; j < SIMULA_FS_DATA_BLOCK_CNT; j++)
		{
			if (fe.blocks[j] == 0) break;
			used_blocks[fe.blocks[j]] = 1;
		}
	}
}
void shut_browsing(int sfs_handle)
{
	free(used_blocks);
}
int get_data_block(int sfs_handle)
{
	int i;

	for (i = sb.data_block_start; i < sb.partition_size; i++)
	{
		if (used_blocks[i] == 0)
		{
			used_blocks[i] = 1;
			return i;
		}
	}
	return -1;
}
void put_data_block(int sfs_handle, int i)
{
	used_blocks[i] = 0;
}

void sfs_list(int sfs_handle)
{
	int i;
	sfs_file_entry_t fe;

	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size, SEEK_SET);
	for (i = 0; i < sb.entry_count; i++)
	{
		read(sfs_handle, &fe, sizeof(sfs_file_entry_t));
		if (!fe.name[0]) continue;
		printf("%-15s  %10d bytes  %c%c%c  %s",
			fe.name, fe.size,
			fe.perms & 04 ? 'r' : '-',
			fe.perms & 02 ? 'w' : '-',
			fe.perms & 01 ? 'x' : '-',
			ctime((time_t *)&fe.timestamp)
			);
	}
}
void sfs_create(int sfs_handle, char *fn)
{
	int i;
	sfs_file_entry_t fe;

	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size, SEEK_SET);
	for (i = 0; i < sb.entry_count; i++)
	{
		read(sfs_handle, &fe, sizeof(sfs_file_entry_t));
		if (!fe.name[0]) break;
		if (strcmp(fe.name, fn) == 0)
		{
			printf("File %s already exists\n", fn);
			return;
		}
	}
	if (i == sb.entry_count)
	{
		printf("No entries left\n", fn);
		return;
	}

	lseek(sfs_handle, -sb.entry_size, SEEK_CUR);

	strncpy(fe.name, fn, 15);
	fe.name[15] = 0;
	fe.size = 0;
	fe.timestamp = time(NULL);
	fe.perms = 07;
	for (i = 0; i < SIMULA_FS_DATA_BLOCK_CNT; i++)
	{
		fe.blocks[i] = 0;
	}

	write(sfs_handle, &fe, sizeof(sfs_file_entry_t));
}
int sfs_lookup(int sfs_handle, char *fn, sfs_file_entry_t *fe)
{
	int i;

	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size, SEEK_SET);
	for (i = 0; i < sb.entry_count; i++)
	{
		read(sfs_handle, fe, sizeof(sfs_file_entry_t));
		if (!fe->name[0]) continue;
		if (strcmp(fe->name, fn) == 0) return i;
	}

	return -1;
}
void sfs_remove(int sfs_handle, char *fn)
{
	int i, block_i;
	sfs_file_entry_t fe;

	if ((i = sfs_lookup(sfs_handle, fn, &fe)) == -1)
	{
		printf("File %s doesn't exist\n", fn);
		return;
	}
	/* Free up all allocated blocks, if any */
	for (block_i = 0; block_i < SIMULA_FS_DATA_BLOCK_CNT; block_i++)
	{
		if (!fe.blocks[block_i])
		{
			break;
		}
		put_data_block(sfs_handle, fe.blocks[block_i]);
	}
	memset(&fe, 0, sizeof(sfs_file_entry_t));

	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size + i * sb.entry_size, SEEK_SET);
	write(sfs_handle, &fe, sizeof(sfs_file_entry_t));
}
void sfs_update(int sfs_handle, char *fn, int *size, int update_ts, int *perms)
{
	int i;
	sfs_file_entry_t fe;

	if ((i = sfs_lookup(sfs_handle, fn, &fe)) == -1)
	{
		printf("File %s doesn't exist\n", fn);
		return;
	}
	if (size) fe.size = *size;
	if (update_ts) fe.timestamp = time(NULL);
	if (perms && (*perms <= 07)) fe.perms = *perms;
	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size + i * sb.entry_size, SEEK_SET);
	write(sfs_handle, &fe, sizeof(sfs_file_entry_t));
}
void sfs_chperm(int sfs_handle, char *fn, int perm)
{
	sfs_update(sfs_handle, fn, NULL, 0, &perm);
}
void sfs_read(int sfs_handle, char *fn)
{
	int i, block_i, already_read, rem_to_read, to_read;
	sfs_file_entry_t fe;

	if ((i = sfs_lookup(sfs_handle, fn, &fe)) == -1)
	{
		printf("File %s doesn't exist\n", fn);
		return;
	}
	if (!(fe.perms & 04))
	{
		printf("File %s doesn't have read permissions\n", fn);
		return;
	}
	already_read = 0;
	rem_to_read = fe.size;
	for (block_i = 0; block_i < SIMULA_FS_DATA_BLOCK_CNT; block_i++)
	{
		if (!fe.blocks[block_i]) break;
		to_read = (rem_to_read >= sb.block_size) ? sb.block_size : rem_to_read;
		lseek(sfs_handle, fe.blocks[block_i] * sb.block_size, SEEK_SET);
		read(sfs_handle, block, to_read);
		write(1, block, to_read);
		already_read += to_read;
		rem_to_read -= to_read;
		if (!rem_to_read) break;
	}
}
void sfs_write(int sfs_handle, char *fn)
{
	int i, cur_read_i, to_read, cur_read, total_size, block_i, free_i;
	sfs_file_entry_t fe;

	if ((i = sfs_lookup(sfs_handle, fn, &fe)) == -1)
	{
		printf("File %s doesn't exist\n", fn);
		return;
	}
	if (!(fe.perms & 02))
	{
		printf("File %s doesn't have write permissions\n", fn);
		return;
	}
	/* Free up all previously allocated blocks, if any */
	for (block_i = 0; block_i < SIMULA_FS_DATA_BLOCK_CNT; block_i++)
	{
		if (!fe.blocks[block_i])
		{
			break;
		}
		put_data_block(sfs_handle, fe.blocks[block_i]);
	}
	/* Let's get data & write */
	cur_read_i = 0;
	to_read = sb.block_size;
	total_size = 0;
	block_i = 0;
	while ((cur_read = read(0, block + cur_read_i, to_read)) > 0)
	{
		if (cur_read == to_read)
		{
			/* Write this block */
			if (block_i == SIMULA_FS_DATA_BLOCK_CNT)
				break; /* File size limit */
			if ((free_i = get_data_block(sfs_handle)) == -1)
				break; /* File system full */
			lseek(sfs_handle, free_i * sb.block_size, SEEK_SET);
			write(sfs_handle, block, sb.block_size);
			fe.blocks[block_i] = free_i;
			block_i++;
			total_size += sb.block_size;
			/* Reset various variables */
			cur_read_i = 0;
			to_read = sb.block_size;
		}
		else
		{
			cur_read_i += cur_read;
			to_read -= cur_read;
		}
	}
	if ((cur_read <= 0) && (cur_read_i))
	{
		/* Write this partial block */
		if ((block_i != SIMULA_FS_DATA_BLOCK_CNT) &&
			((fe.blocks[block_i] = get_data_block(sfs_handle)) != -1))
		{
			lseek(sfs_handle, fe.blocks[block_i] * sb.block_size, SEEK_SET);
			write(sfs_handle, block, cur_read_i);
			total_size += cur_read_i;
		}
	}

	fe.size = total_size;
	fe.timestamp = time(NULL);
	lseek(sfs_handle, sb.entry_table_block_start * sb.block_size + i * sb.entry_size, SEEK_SET);
	write(sfs_handle, &fe, sizeof(sfs_file_entry_t));
}

void usage(void)
{
	printf("Supported commands:\n");
	printf("\t?\tquit\tlist\tcreate <file>\tremove <file>\n");
	printf("\t\tchperm <0-7> <file>\tread <file>\twrite <file>\n");
}

void browse_sfs(int sfs_handle)
{
	int done;
	char cmd[256], *fn;
	int ret, perm;

	done = 0;

	printf("Welcome to SFS Browsing Shell v3.0\n\n");
	printf("Block size     : %d bytes\n", sb.block_size);
	printf("Partition size : %d blocks\n", sb.partition_size);
	printf("File entry size: %d bytes\n", sb.entry_size);
	printf("Entry tbl size : %d blocks\n", sb.entry_table_size);
	printf("Entry count    : %d\n", sb.entry_count);
	printf("\n");
	init_browsing(sfs_handle);
	while (!done)
	{
		printf(" $> ");
		ret = scanf("%[^\n]", cmd);
		if (ret < 0)
		{
			done = 1;
			printf("\n");
			continue;
		}
		else
		{
			getchar();
			if (ret == 0) continue;
		}
		if (strcmp(cmd, "?") == 0)
		{
			usage();
			continue;
		}
		else if (strcmp(cmd, "quit") == 0)
		{
			done = 1;
			continue;
		}
		else if (strcmp(cmd, "list") == 0)
		{
			sfs_list(sfs_handle);
			continue;
		}
		else if (strncmp(cmd, "create", 6) == 0)
		{
			if (cmd[6] == ' ')
			{
				fn = cmd + 7;
				while (*fn == ' ') fn++;
				if (*fn != '\0')
				{
					sfs_create(sfs_handle, fn);
					continue;
				}
			}
		}
		else if (strncmp(cmd, "remove", 6) == 0)
		{
			if (cmd[6] == ' ')
			{
				fn = cmd + 7;
				while (*fn == ' ') fn++;
				if (*fn != '\0')
				{
					sfs_remove(sfs_handle, fn);
					continue;
				}
			}
		}
		else if (strncmp(cmd, "chperm", 6) == 0)
		{
			if (cmd[6] == ' ')
			{
				perm = cmd[7] - '0';
				if ((0 <= perm) && (perm <= 7) &&  (cmd[8] == ' '))
				{
					fn = cmd + 9;
					while (*fn == ' ') fn++;
					if (*fn != '\0')
					{
						sfs_chperm(sfs_handle, fn, perm);
						continue;
					}
				}
			}
		}
		else if (strncmp(cmd, "read", 4) == 0)
		{
			if (cmd[4] == ' ')
			{
				fn = cmd + 5;
				while (*fn == ' ') fn++;
				if (*fn != '\0')
				{
					sfs_read(sfs_handle, fn);
					continue;
				}
			}
		}
		else if (strncmp(cmd, "write", 5) == 0)
		{
			if (cmd[5] == ' ')
			{
				fn = cmd + 6;
				while (*fn == ' ') fn++;
				if (*fn != '\0')
				{
					sfs_write(sfs_handle, fn);
					continue;
				}
			}
		}
		printf("Unknown/Incorrect command: %s\n", cmd);
		usage();
	}
	shut_browsing(sfs_handle);
}

int main(int argc, char *argv[])
{
	char *sfs_file;
	int sfs_handle;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <partition's device file>\n", argv[0]);
		return 1;
	}
	sfs_file = argv[1];
	sfs_handle = open(sfs_file, O_RDWR);
	if (sfs_handle == -1)
	{
		fprintf(stderr, "Unable to browse SFS over %s\n", sfs_file);
		return 2;
	}
	read(sfs_handle, &sb, sizeof(sfs_super_block_t));
	if (sb.type != SIMULA_FS_TYPE)
	{
		fprintf(stderr, "Invalid SFS detected. Giving up.\n");
		close(sfs_handle);
		return 3;
	}
	browse_sfs(sfs_handle);
	close(sfs_handle);
	return 0;
}
