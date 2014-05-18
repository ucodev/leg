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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#include "archdefs.h"

/* POSIX queue configuration */
#define PQ_IPC_KEY	1234
#define PQ_MSG_SZ	1024

/* Data structures */
struct config_vm {
	leg_addr_t ram;			/* System RAM */
	char *stor[HW_STOR_MAX];	/* Storage */
};

struct config {
	struct config_vm vm;
};

/* External variables */
extern struct config config;

/* Prototypes */
void config_init(const char *);
void config_destroy(void);


#endif
