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
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>

#include "config.h"
#include "archdefs.h"
#include "io.h"
#include "interrupt.h"
#include "mm.h"
#include "debug.h"
#include "pqueue.h"

volatile struct io io;

static void _io_storage_init(void) {
	int i, count = 0;

	for (i = 0; i < HW_STOR_MAX; i++) {
		if (!config.vm.stor[i])
			continue;

		count++;

		if ((io.fdstor[i] = open(config.vm.stor[i], O_RDWR)) < 0) {
			printf("Failed to open storage ID '%d': %m\n", i);
			exit(EXIT_FAILURE);
		}
	}

	if (!count) {
		puts("No storage devices found. Aborting...");
		exit(EXIT_FAILURE);
	}

#ifdef DEBUG
	printf("\n\t* Found %d Storage Devices.\n\t", count);
#endif
}

static void _io_storage_destroy(void) {
	int i;

	for (i = 0; i < HW_STOR_MAX; i++) {
		if (!config.vm.stor[i])
			continue;

		close(io.fdstor[i]);
	}
}

int io_display_write(uint8_t byte) {
	pqueue_reset_buf(pq);

	pq->msg_buf.data[0] = byte;
	pq->data_size = 1;

	if (pqueue_send(pq, INTR_0A) < 0)
		return -1;

	return 0;
}

int io_storage_write_extended(uint16_t storid, leg_addr_t addr, uint64_t offset, size_t size) {
	if (lseek64(io.fdstor[storid], offset, SEEK_SET) < 0)
		return -1;

	if (write(io.fdstor[storid], (void *) (mm + addr), size) != size)
		return -1;

	return 0;
}

int io_storage_write(uint16_t storid, leg_addr_t addr, size_t offset, size_t size) {
	if (lseek(io.fdstor[storid], offset, SEEK_SET) < 0)
		return -1;

	if (write(io.fdstor[storid], (void *) (mm + addr), size) != size)
		return -1;

	return 0;
}

int io_storage_read_extended(uint16_t storid, leg_addr_t addr, uint64_t offset, size_t size) {
	if (lseek64(io.fdstor[storid], offset, SEEK_SET) < 0)
		return -1;

	if (read(io.fdstor[storid], (void *) (mm + addr), size) != size)
		return -1;

	return 0;
}

int io_storage_read(uint16_t storid, leg_addr_t addr, size_t offset, size_t size) {
	if (lseek(io.fdstor[storid], offset, SEEK_SET) < 0)
		return -1;

	if (read(io.fdstor[storid], (void *) (mm + addr), size) != size)
		return -1;

	return 0;
}

void io_init(void) {
	_io_storage_init();
}

void io_destroy(void) {
	_io_storage_destroy();
}

