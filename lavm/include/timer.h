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

#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>

/* Data structures */
struct timer {
	uint8_t id;
	uint8_t granularity;
	uint32_t tte;
};

/* Prototypes */
void *timer_setup(void *targ);
void timer_destroy(void);

/* External */
extern pthread_t timer_list[TIMER_NUM_MAX];

#endif
