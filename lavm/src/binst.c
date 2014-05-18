/*
   Copyright 2012-2014 Pedro A. Hortas (pah@ucodev.org)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <arpa/inet.h>

#define ADDR_BOOT	0x000
#define ADDR_KERNEL	0x800

#define BTYPE_BOOT	1
#define BTYPE_KERNEL	2

struct cmdline {
	uint8_t btype;
	FILE *fpbin;
	FILE *fpstor;
	uint32_t addr;
};

static struct cmdline _cmdline = { 0, NULL, NULL, 0 };

static uint8_t str_to_btype(const char *str) {
	if (!strcmp(str, "kernel"))
		return BTYPE_KERNEL;

	if (!strcmp(str, "bootloader"))
		return BTYPE_BOOT;

	return 0;
}

static char *btype_to_str(uint8_t btype) {
	switch (btype) {
		case BTYPE_BOOT:   return "Bootloader";
		case BTYPE_KERNEL: return "Kernel";
	}

	return "<unknown>";
}

static uint32_t btype_to_addr(uint8_t btype) {
	switch (btype) {
		case BTYPE_BOOT:   return ADDR_BOOT;
		case BTYPE_KERNEL: return ADDR_KERNEL;
	}

	return 0;
}

static long file_size(FILE *fp) {
	long size = 0;

	if (fseek(fp, 0, SEEK_END) < 0) {
		printf("fseek(): %m\n");
		return -1;
	}

	size = ftell(fp);

	if (fseek(fp, 0, SEEK_SET) < 0) {
		printf("fseek(): %m\n");
		return -1;
	}

	return size;
}

static int file_copy(FILE *fpsrc, FILE *fpdst) {
	int ch;

	while (!feof(fpsrc)) {
		ch = fgetc(fpsrc);

		if (fputc(ch, fpdst) == EOF) {
			printf("Write failed at address 0x%.8X\n", (uint32_t) ftell(fpdst));
			return -1;
		}
	}

	return 0;
}

static int binst(FILE *fpbin, FILE *fpstor, uint32_t btype, uint32_t addr) {
	long bsize = 0;

	if (fseek(fpstor, addr, SEEK_SET) < 0) {
		printf("fseek(): %m\n");
		return -1;
	}

	if ((bsize = file_size(fpbin)) < 0) {
		printf("Unable to get %s size: %m\n", btype_to_str(btype));
		return -1;
	}

	printf("%s size: %ld bytes\n", btype_to_str(btype), bsize);
	printf("%s storage address space: 0x%.8X-0x%.8X\n", btype_to_str(btype), addr + (btype == BTYPE_KERNEL ? 4 : 0), (uint32_t) (addr + bsize));
	printf("Installing %s at storage address: 0x%.8X...\n", btype_to_str(btype), addr + (btype == BTYPE_KERNEL ? 4 : 0));

	if (btype == BTYPE_KERNEL) {
		/* Kernel needs a 32-bit size prefix */
		printf("Setting 32-bit kernel size prefix 0x%.8X at storage address: 0x%.8X...\n", (uint32_t) bsize, addr);

		fwrite(&(uint32_t) { htonl(bsize) }, 4, 1, fpstor);
	}

	if (file_copy(fpbin, fpstor) < 0)
		return -1;

	return 0;
}

static void usage(char **argv) {
	printf("Usage: %s <bin> <type> <storage>\n\n", argv[0]);
	puts("Arguments:");
	puts("\t<bin>     - Bootloader or kernel binary to be installed");
	puts("\t<type>    - kernel, bootloader");
	puts("\t<storage> - Target storage device file\n");
}

static void syntax(int argc, char **argv, struct cmdline *cmdline) {
	if (argc != 4) {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	/* Process command line arguments */
	if (!(cmdline->btype = str_to_btype(argv[2]))) {
		printf("Invalid type: %s\n", argv[2]);
		usage(argv);
		exit(EXIT_FAILURE);
	}

	cmdline->addr = btype_to_addr(cmdline->btype);

	if (!(cmdline->fpbin = fopen(argv[1], "rb"))) {
		printf("Unable to open file '%s' for reading: %m\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if (!(cmdline->fpstor = fopen(argv[3], "r+b"))) {
		printf("Unable to open file '%s' for writing: %m\n", argv[3]);
		exit(EXIT_FAILURE);
	}
}

static void destroy(struct cmdline *cmdline) {
	fclose(cmdline->fpbin);
	fclose(cmdline->fpstor);
}

int main(int argc, char *argv[]) {
	syntax(argc, argv, &_cmdline);

	if (binst(_cmdline.fpbin, _cmdline.fpstor, _cmdline.btype, _cmdline.addr) < 0) {
		printf("%s installation failed (start address: 0x%.8X)\n", btype_to_str(_cmdline.btype), _cmdline.addr);
		exit(EXIT_FAILURE);
	}

	puts("Done.");

	destroy(&_cmdline);

	return 0;
}

