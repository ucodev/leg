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

#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1
#define DEBUG_ENTER_OPERAND_STATUS 0
#define DEBUG_LEAVE_OPERAND_STATUS 0

#include <stdint.h>

#include "archdefs.h"

/* Prototypes */
volatile char *const debug_operand_to_str(leg_addr_t, uint8_t);
int debug_instruction_enter(const char *, leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
int debug_instruction_leave(const char *, leg_addr_t, leg_addr_t, uint8_t, uint8_t, uint8_t);
void debug_interrupt_caught(uint8_t);

#endif
