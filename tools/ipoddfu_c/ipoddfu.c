#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

#include "usb.h"
#include "dfu.h"
#include "crc32.h"
#include "misc.h"
#include "ipoddfu.h"

int main(int argc, char *argv[]) {
	int res = 0;
	unsigned char reattach = 0, *data;
	FILE *fp;
	long int size;
	unsigned int checksum;
		
	if (2 != argc) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		
		return 1;
	}
	
	fp = fopen(argv[1], "r");
	
	if (!fp) {
		perror("fopen");
		
		return 1;
	}
	
	size = fgetsize(fp);
	
	if (-1L == size) {
		return 1;
	}
	
	data = (unsigned char *) malloc(size + 4);
	
	if ((unsigned long int) size != fread(data, sizeof(unsigned char), size, fp)) {
		perror("fread");
		
		return 1;
	}
	
	if (fclose(fp)) {
		perror("fclose");
		
		return 1;
	}
	
	crc32_init();
	
	checksum = crc32(data, size, CRC32_DEFAULT_SEED);
	
	memcpy(data + size, &checksum, 4);
	
	res = usb_init();
	
	if (LIBUSB_SUCCESS == res) {
		res = usb_find(&reattach);
	}
	
	if (LIBUSB_SUCCESS == res) {
		res = dfu_send((unsigned long int) size + 4, data);
	}
	
	if (data) {
		free(data);
	}
	
	if (0 != res) {
		print_error(res);
	}
	
	if (usb_handle) {
		usb_close(reattach);
	}
	
	if (usb_ctx) {
		usb_exit();
	}
	
	if (res < 0) {
		res = -res;
	}
	
	return res;
}
