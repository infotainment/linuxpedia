#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int
main (void)
{
    int h;
    int x;
    char ch, val;

    h = open ("/dev/module4", O_RDWR);
    if (h == -1) {
        printf ("cannot open module!\n");
        return 1;
    }

    for (x = 0; x < 10; x++) {
            ch = (char)x;
            write (h, &ch, 1);
            read (h, &val, 1);
            printf ("we wrote: %d, driver says we just wrote: %d\n", ch, val);
            usleep (250000);
    }

    ch = 0;
    write (h, &ch, 1);
    close (h);

    return 0;
}

