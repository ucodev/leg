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

#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>

#include "archdefs.h"

#pragma pack(push)
#pragma pack(1)
/* Data structures */
struct reg {
	leg_addr_t rip;	/* Instruction Pointer register */
	leg_addr_t rst;	/* Status register */
	leg_addr_t rff;	/* Fault Flags */
	leg_addr_t rfa;	/* Fault Handling Routine Address */
	leg_addr_t rbt;	/* Base Task Register */
	leg_addr_t rct;	/* Current Task Register */
	leg_addr_t rpa;	/* Paging Address Register */
	leg_addr_t rra;	/* Return Address Register */
	leg_addr_t rsa;	/* Stack address Register */
	leg_addr_t rcmp;	/* Comparator register */
	leg_addr_t rlgic;	/* Logic register */
	leg_addr_t rarth;	/* Arithmetic register */

	/* 8 General Propose registers */
	leg_addr_t rgp1, rgp2, rgp3, rgp4;
	leg_addr_t rgp5, rgp6, rgp7, rgp8;

	/* 4 Arithmetic Logic registers */
	leg_addr_t ral1, ral2, ral3, ral4;

	/* 4 Floating Point registers */
	leg_addr_t rfp1, rfp2, rfp3, rfp4;
};
#pragma pack(pop)

/* External declarations */
extern volatile struct reg regs;

extern volatile leg_addr_t *const regs_list[REG_NUM];

/* Prototypes */
volatile void *register_decode(leg_addr_t regid);
int register_grant_any(leg_addr_t regid);
int register_grant_arth(leg_addr_t regid);
int register_grant_rfp(leg_addr_t regid);
int register_is_rfp(leg_addr_t regid);

#endif

