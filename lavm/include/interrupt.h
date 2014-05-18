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

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#include "archdefs.h"

/* Interrupt Vector Entry */
#pragma pack(push)
#pragma pack(1)
struct intrv_entry {
	uint32_t flags;			// bit 0: Privilege Level
	leg_addr_t handler_addr;
};
#pragma pack(pop)


/* External variables */
volatile struct intrv_entry intrv[INTERRUPT_VECTOR_SIZE];

/* Prototypes */
void interrupt_hw_check(void);
void interrupt_intvr_handler(uint8_t);
void interrupt_int01(leg_addr_t, leg_addr_t, leg_addr_t);
void interrupt_int03(void);
void interrupt_int09(uint32_t);
void interrupt_int0a(leg_addr_t);
void interrupt_int0b(leg_addr_t, leg_addr_t, leg_addr_t, leg_addr_t, leg_addr_t);
void interrupt_int0d(leg_addr_t);
void interrupt_int0f(leg_addr_t, leg_addr_t, leg_addr_t);
void interrupt_int10(uint8_t);

#endif
