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

#include "config.h"
#include "init.h"
#include "mm.h"
#include "io.h"
#include "timer.h"
#include "pqueue.h"


void vm_destroy(void) {
	timer_destroy();
	io_destroy();
	mm_destroy();
	config_destroy();

	pqueue_destroy(pq);

	exit(EXIT_SUCCESS);
}
	
static void _syntax(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: %s <vm config dir>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	_syntax(argc, argv);

	init_vm(argv[1]);

	return 0;
}

