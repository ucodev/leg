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
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "pqueue.h"

static struct pq_ipc *pq_keyb;

static void _keyboard_pqueue_init(void) {
	if (!(pq_keyb = pqueue_init(PQ_IPC_KEY, 0600))) {
		puts("Failed to initialize POSIX queues (pqueue_init()).");
		exit(EXIT_FAILURE);
	}

	if (pqueue_create(pq_keyb) < 0) {
		puts("Failed to initialize POSIX queues (pqueue_create()).");
		exit(EXIT_FAILURE);
	}
}

static void _keyboard_pqueue_destroy(void) {
	pqueue_destroy(pq_keyb);
}

uint32_t keyboard_input(void) {
	uint32_t ch = 0;

	/* NOTE: Supress compiler warning on unused write() return value */
	(ssize_t [1]) { }[0] = read(STDIN_FILENO, &ch, 1);

	return ch;
}

void keyboard_dispatch(uint32_t keybcode) {
	pqueue_reset_buf(pq_keyb);

	memcpy(pq_keyb->msg_buf.data, &keybcode, 4);
	pq_keyb->data_size = 4;

	pqueue_send(pq_keyb, INTR_09);
}

int keyboard_init(void) {
	_keyboard_pqueue_init();

	return 0;
}

void keyboard_destroy(void) {
	_keyboard_pqueue_destroy();
}

