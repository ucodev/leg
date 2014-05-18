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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

#include "archdefs.h"

/* Data Structures */
struct instruction {
	leg_addr_t size;		/* size of instruction, in bytes */
	void (*doop) (leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t); /* Execute instruction */
	
};

/* Prototypes */
void cpvr(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void cpvl(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void cpr(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void cprr(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void cmp(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void jmp(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void call(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void ret(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void arth(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void lgic(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void intr(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void ceb(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void nop(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void ltsk(leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);

/* External variables */
const struct instruction instruction[INSTRUCTION_SET_SIZE];

#endif

