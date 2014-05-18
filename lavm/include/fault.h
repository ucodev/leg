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

#ifndef FAULT_H
#define FAULT_H

#include <stdio.h>
#include <stdint.h>

/* Prototypes */
void fault_mc(void);
void fault_bad_mm(uint32_t);
void fault_bad_reg(uint32_t);
void fault_bad_reg_val(uint32_t, uint32_t);
void fault_bad_oper_val(uint32_t);
void fault_intr_failed(uint32_t);
void fault_illegal_interrupt(uint32_t);
void fault_illegal_instruction(void);
void fault_fpu(void);
void fault_alu(void);
void fault_no_page(uint32_t);
void fault_page_perm(uint32_t, uint32_t);
void fault_no_priv(void);
void fault_io_op(void);

#endif

