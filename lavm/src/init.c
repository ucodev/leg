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
#include <string.h>
#include <unistd.h>

#include "archdefs.h"
#include "register.h"
#include "mm.h"
#include "run.h"
#include "sighandler.h"
#include "io.h"
#include "config.h"
#include "pqueue.h"

static void _init_config(const char *path) {
	config_init(path);
}

static void _init_registers(void) {
	memset((void *) &regs, 0, sizeof(struct reg));

	regs.rip = MM_ZONE_NORMAL;
}

static void _init_cpu(void) {
	printf("Initializing CPU... ");

	_init_registers();

	puts("OK");

}

static void _init_io(void) {
	printf("Initializing I/O... ");

	io_init();

	puts("OK");
}

static void _init_mm(void) {
	printf("Initializing RAM... ");

	if (mm_init() < 0) {
		puts("Failed to allocate VM memory");
		exit(EXIT_FAILURE);
	}

	printf("%u bytes OK\n", config.vm.ram);
}

static void _init_bootloader(void) {
	printf("Loading Bootloader... ");

	if (!config.vm.stor[0]) {
		printf("Unable to find storage ID 0.\n");
		exit(EXIT_FAILURE);
	}

	if (read(io.fdstor[0], (void *) (mm + MM_ZONE_NORMAL), 2048) != 2048) {
		puts("Failed to load bootloader.");
		exit(EXIT_FAILURE);
	}

	puts("OK");
}

static void _init_run(void) {
	puts("Starting VM...");

	run_start();
}

static void _init_signals(void) {
	sighandler_init();
}

static void _init_pqueue(void) {
	if (!(pq = pqueue_init(PQ_IPC_KEY, IPC_CREAT | 0600))) {
		puts("Failed to initialize POSIX queues (pqueue_init()).");
		exit(EXIT_FAILURE);
	}

	if (pqueue_create(pq) < 0) {
		puts("Failed to initialize POSIX queues (pqueue_create()).");
		exit(EXIT_FAILURE);
	}
}

void init_vm(const char *path) {
	_init_config(path);

	_init_signals();

	_init_pqueue();

	_init_cpu();

	_init_mm();

	_init_io();

	_init_bootloader();

	_init_run();
}


