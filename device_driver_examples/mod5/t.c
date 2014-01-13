#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <module5/module5.h>

int
main (void)
{
	int h;
	int x;

	h = open ("/dev/module5", O_RDWR);
	if (h == -1) {
		printf ("cannot open module!\n");
		return 1;
	}	/* endif */

	// call up reset request, no parameters
	ioctl (h, MODULE5_IOCRESET);

	for (x = 0; x < 16; x++) {
		// call up write pattern request, supply parameter
		ioctl (h, MODULE5_IOCSPATTERN, &x);
		sleep (1);
	}	/* end for */

	close (h);

	return 0;
}	/* end main */

