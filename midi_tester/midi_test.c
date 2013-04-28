#include <stdio.h>
#include <sndio.h>
#include <stdlib.h>

int
main(void)
{
	struct mio_hdl *mh;
	unsigned char byte;

	if ((mh = mio_open("rmidi/0", MIO_IN, 0)) == NULL) {
		printf("mio_open failed\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((mio_read(mh, &byte, 1)) != 1) {
			printf("mio_read failed\n");
			exit(EXIT_FAILURE);
		}
		printf("Got byte: 0x%02x\n", byte);
	}

	mio_close(mh);
}
