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

#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>

#include "archdefs.h"

/* Data structures */
struct io {
	int fdstor[HW_STOR_MAX];	/* Storage file descriptor array */
};

/* External variables */
extern volatile struct io io;

/* Prototypes */
int io_display_write(uint8_t);
int io_storage_write_extended(uint16_t, leg_addr_t, uint64_t, size_t);
int io_storage_write(uint16_t, leg_addr_t, size_t, size_t);
int io_storage_read_extended(uint16_t, leg_addr_t, uint64_t, size_t);
int io_storage_read(uint16_t, leg_addr_t, size_t, size_t);
void io_init(void);
void io_destroy(void);

#endif

