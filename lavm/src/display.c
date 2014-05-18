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

#include "archdefs.h"
#include "config.h"
#include "pqueue.h"

static struct pq_ipc *pq_disp;

static void _display_pqueue_init(void) {
	if (!(pq_disp = pqueue_init(PQ_IPC_KEY, 0600))) {
		puts("Failed to initialize POSIX queues (pqueue_init()).");
		exit(EXIT_FAILURE);
	}

	if (pqueue_create(pq_disp) < 0) {
		puts("Failed to initialize POSIX queues (pqueue_create()).");
		exit(EXIT_FAILURE);
	}
}

static void _display_pqueue_destroy(void) {
	pqueue_destroy(pq_disp);
}

char display_receive(void) {
	char ch = 0;

	if (pqueue_recv(pq_disp, INTR_0A) > 0)
		ch = pq_disp->msg_buf.data[0];

	pqueue_reset_buf(pq_disp);

	return ch;
}

int display_init(void) {
	_display_pqueue_init();

	return 0;
}

void display_destroy(void) {
	_display_pqueue_destroy();
}

