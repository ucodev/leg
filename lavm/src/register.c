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

#include <stdint.h>

#include "archdefs.h"
#include "register.h"
#include "fault.h"


/* Globals */
volatile struct reg regs;

volatile leg_addr_t *const regs_list[REG_NUM] = {
	&regs.rip, &regs.rst, &regs.rff, &regs.rfa,
	&regs.rbt, &regs.rct, &regs.rpa,
	&regs.rra, &regs.rsa,
	&regs.rcmp, &regs.rlgic, &regs.rarth,
	&regs.rgp1, &regs.rgp2, &regs.rgp3, &regs.rgp4,
	&regs.rgp5, &regs.rgp6, &regs.rgp7, &regs.rgp8,
	&regs.ral1, &regs.ral2, &regs.ral3, &regs.ral4,
	&regs.rfp1, &regs.rfp2, &regs.rfp3, &regs.rfp4
};

volatile void *register_decode(leg_addr_t regid) {
	return regs_list[regid / 4];
}

int register_grant_any(leg_addr_t regid) {
	if ((regid > REG_RFP4) || (regid % 4)) {
		fault_bad_reg(regid);
		return 0;
	}

	return 1;
}

int register_grant_arth(leg_addr_t regid) {
	if (((regid < REG_RFP1) || (regid > REG_RFP4)) && ((regid < REG_RAL1) || (regid > REG_RAL4)) && (regid != REG_RSA)) {
		fault_bad_reg(regid);
		return 0;
	}

	return 1;
}

int register_grant_rfp(leg_addr_t regid) {
	if ((regid < REG_RFP1) || (regid > REG_RFP4) || (regid % 4)) {
		fault_bad_reg(regid);
		return 0;
	}

	return 1;
}

int register_is_rfp(leg_addr_t regid) {
	return ((regid >= REG_RFP1) && (regid <= REG_RFP4));
}

