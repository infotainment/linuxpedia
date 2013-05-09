#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "sfs_ds.h"

sfs_super_block_t sb;

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
void sfs_remove(int sfs_handle, char *fn)
{
    int i;
    sfs_file_entry_t fe;

    lseek(sfs_handle, sb.entry_table_block_start * sb.block_size, SEEK_SET);
    for (i = 0; i < sb.entry_count; i++)
    {
        read(sfs_handle, &fe, sizeof(sfs_file_entry_t));
        if (!fe.name[0]) continue;
        if (strcmp(fe.name, fn) == 0) break;
    }
    if (i == sb.entry_count)
    {
        printf("File %s doesn't exist\n", fn);
        return;
    }

    lseek(sfs_handle, -sb.entry_size, SEEK_CUR);

    memset(&fe, 0, sizeof(sfs_file_entry_t));
    write(sfs_handle, &fe, sizeof(sfs_file_entry_t));
}

void browse_sfs(int sfs_handle)
{
    int done;
    char cmd[256], *fn;
    int ret;

    done = 0;

    printf("Welcome to SFS Browsing Shell v1.0\n\n");
    printf("Block size     : %d bytes\n", sb.block_size);
    printf("Partition size : %d blocks\n", sb.partition_size);
    printf("File entry size: %d bytes\n", sb.entry_size);
    printf("Entry tbl size : %d blocks\n", sb.entry_table_size);
    printf("Entry count    : %d\n", sb.entry_count);
    printf("\n");
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
            printf("Supported commands:\n");
            printf("\t?\tquit\tlist\tcreate\tremove\n");
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
        printf("Unknown/Incorrect command: %s\n", cmd);
        printf("Supported commands:\n");
        printf("\t?\tquit\tlist\tcreate <file>\tremove <file>\n");
    }
}

int main(int argc, char *argv[])
{
    char *sfs_file = SIMULA_DEFAULT_FILE;
    int sfs_handle;

    if (argc > 2)
    {
        fprintf(stderr, "Incorrect invocation. Possibilities are:\n");
        fprintf(stderr, "\t%s /* Picks up %s as the default partition_file */\n",
                argv[0], SIMULA_DEFAULT_FILE);
        fprintf(stderr, "\t%s [ partition_file ]\n", argv[0]);
        return 1;
    }
    if (argc == 2)
    {
        sfs_file = argv[1];
    }
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

