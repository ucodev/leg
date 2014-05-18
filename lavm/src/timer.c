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
#include <time.h>
#include <pthread.h>

#include "archdefs.h"
#include "fault.h"
#include "timer.h"
#include "pqueue.h"

pthread_t timer_list[TIMER_NUM_MAX];

static void _timer_expired(uint8_t id) {
	pqueue_reset_buf(pq);

	memcpy(pq->msg_buf.data, &id, 1);
	pq->data_size = 1;

	pqueue_send(pq, INTR_10);
}

void *timer_setup(void *targ) {
	struct timer *timer = (struct timer *) targ;

	switch (timer->granularity) {
		case 1: if (nanosleep((struct timespec [1]) { { 0, timer->tte } }, NULL) < 0)
				fault_mc();
			break;
		case 2: if (usleep(timer->tte) < 0)
				fault_mc();
			break;
		case 4: if (usleep(timer->tte * 1000) < 0) // FIXME: *1000 :(
				fault_mc();
			break;
		case 8: if (sleep(timer->tte))
				fault_mc();
			break;
	}

	_timer_expired(timer->id);

	free(timer);	// Alloc'd on main thread (at interrupt.c)

	return NULL;
}

void timer_destroy(void) {
	int i;

	for (i = 0; i < TIMER_NUM_MAX; i++)
		pthread_cancel(timer_list[i]);
}

