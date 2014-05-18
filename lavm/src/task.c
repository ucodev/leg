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

#include <string.h>

#include <arpa/inet.h>

#include "archdefs.h"
#include "register.h"
#include "mm.h"
#include "paging.h"
#include "fault.h"

/* TODO: Not implemented on 64-bit yet... htonll() needed and the register IDs
 * must be multiplied by 2 since they're being added to base mm.
 */


void task_save_rct(void) {
	leg_addr_t prct = regs.rct; // Assume RCT value as Physical Address

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prct = paging_get_paddr(prct, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RCT Address belongs to normal zone */
	if (!mm_grant_zone_normal(prct))
		return;

	/* Control Registers */
	*((leg_addr_t *) ((char *) mm) + prct + REG_RIP) = htonl(regs.rip);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RST) = htonl(regs.rst);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFF) = htonl(regs.rff);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFA) = htonl(regs.rfa);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RBT) = htonl(regs.rbt);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RCT) = htonl(regs.rct);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RPA) = htonl(regs.rpa);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RRA) = htonl(regs.rra);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RSA) = htonl(regs.rsa);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RCMP) = htonl(regs.rcmp);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RLGIC) = htonl(regs.rlgic);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RARTH) = htonl(regs.rarth);

	/* General Purpose Registers */
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP1) = htonl(regs.rgp1);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP2) = htonl(regs.rgp2);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP3) = htonl(regs.rgp3);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP4) = htonl(regs.rgp4);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP5) = htonl(regs.rgp5);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP6) = htonl(regs.rgp6);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP7) = htonl(regs.rgp7);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RGP8) = htonl(regs.rgp8);

	/* Arithmetic/Logic Registers */
	*((leg_addr_t *) ((char *) mm) + prct + REG_RAL1) = htonl(regs.ral1);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RAL2) = htonl(regs.ral2);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RAL3) = htonl(regs.ral3);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RAL4) = htonl(regs.ral4);

	/* Floating Point Registers */
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFP1) = htonl(regs.rfp1);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFP2) = htonl(regs.rfp2);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFP3) = htonl(regs.rfp3);
	*((leg_addr_t *) ((char *) mm) + prct + REG_RFP4) = htonl(regs.rfp4);
}

void task_load_rct(void) {
	leg_addr_t prct = regs.rct; // Assume RCT value as Physical Address

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prct = paging_get_paddr(prct, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RCT Address belongs to normal zone */
	if (!mm_grant_zone_normal(prct))
		return;

	/* Control Registers */
	regs.rip = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RIP));
	regs.rst = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RST));
	regs.rff = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFF));
	regs.rfa = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFA));
	regs.rbt = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RBT));
	regs.rct = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RCT));
	regs.rpa = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RPA));
	regs.rra = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RRA));
	regs.rsa = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RSA));
	regs.rcmp = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RCMP));
	regs.rlgic = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RLGIC));
	regs.rarth = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RARTH));

	/* General Purpose Registers */
	regs.rgp1 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP1));
	regs.rgp2 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP2));
	regs.rgp3 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP3));
	regs.rgp4 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP4));
	regs.rgp5 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP5));
	regs.rgp6 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP6));
	regs.rgp7 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP7));
	regs.rgp8 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RGP8));

	/* Arithmetic/Logic Registers */
	regs.ral1 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RAL1));
	regs.ral2 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RAL2));
	regs.ral3 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RAL3));
	regs.ral4 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RAL4));

	/* Floating Point Registers */
	regs.rfp1 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFP1));
	regs.rfp2 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFP2));
	regs.rfp3 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFP3));
	regs.rfp4 = ntohl(*((leg_addr_t *) ((char *) mm) + prct + REG_RFP4));
}

void task_save_rbt(void) {
	leg_addr_t prbt = regs.rbt; // Assume RBT value as Physical Address

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prbt = paging_get_paddr(prbt, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RBT Address belongs to normal zone */
	if (!mm_grant_zone_normal(prbt))
		return;

	/* NOTE: Only control registers are saved to RBT Task Context
	 *	 except RIP, RFF and RCT
	 */
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RST) = htonl(regs.rst);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RFA) = htonl(regs.rfa);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RBT) = htonl(regs.rbt);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RPA) = htonl(regs.rpa);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RRA) = htonl(regs.rra);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RSA) = htonl(regs.rsa);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RCMP) = htonl(regs.rcmp);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RLGIC) = htonl(regs.rlgic);
	*((leg_addr_t *) ((char *) mm) + prbt + REG_RARTH) = htonl(regs.rarth);
}

void task_load_rbt(void) {
	leg_addr_t prbt = regs.rbt; // Assume RBT value as Physical Address

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prbt = paging_get_paddr(prbt, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RBT Address belongs to normal zone */
	if (!mm_grant_zone_normal(prbt))
		return;

	/* NOTE: Only control registers are loaded from RBT Task Context
	 *	 except RIP, RFF and RCT
	 */
	regs.rst = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RST));
	regs.rfa = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RFA));
	regs.rbt = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RBT));
	regs.rpa = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RPA));
	regs.rra = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RRA));
	regs.rsa = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RSA));
	regs.rcmp = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RCMP));
	regs.rlgic = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RLGIC));
	regs.rarth = ntohl(*((leg_addr_t *) ((char *) mm) + prbt + REG_RARTH));

	/* RBT Task Structure must have Privilege Level 0 */
	if (regs.rst & REG_RST_BIT_LOWPRIV)
		fault_mc();
}

