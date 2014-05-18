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
#include <stdint.h>

#include <arpa/inet.h>

#include "archdefs.h"
#include "register.h"
#include "instruction.h"
#include "interrupt.h"
#include "run.h"
#include "mm.h"
#include "fault.h"
#include "config.h"
#include "debug.h"
#include "paging.h"

volatile struct run run;

void run_start(void) {
	leg_addr_t prip;	// physical RIP
	leg_addr_t opcode_oper1, opcode_oper2;
	uint8_t opcode_size, opcode_id, operand1_type, operand2_type;

	for (;;) {
		opcode_size = 0;
		opcode_id = 0;
		opcode_oper1 = 0;
		opcode_oper2 = 0;

		/* Assume operand types as register by default */
		operand1_type = OPERAND_TYPE_REG;
		operand2_type = OPERAND_TYPE_REG;

		/* Always assume regs.rip is in the physical address space
		 * If it isn't, the next condition compound will translate it.
		 */
		prip = regs.rip;

		/* If paging is being used, translate logical address to
		 * physical address
		 */
		if (regs.rst & REG_RST_BIT_PAGING) {
			/* Get physical address and grant page is executable */
			if (!(prip = paging_get_paddr(regs.rip, PAGE_PERM_EXEC)))
				continue;
		}

		/* Sanity check - Check normal zone and RAM boundaries */
		if (!mm_grant_zone_normal(prip))
			continue;

		/* Extract opcode */
		run.opcode = ntohl(*((uint32_t *) &((char *) mm)[prip]));

		/* Extract instruction id and respective operands */
		opcode_size += ARCH_ADDR_BITS >> 3;
		opcode_id = run.opcode & 0xFF;
		opcode_oper1 = (run.opcode & 0xFF00) >> 8;
		opcode_oper2 = (run.opcode & 0xFF0000) >> 16;

		/* Another sanity check */
		if (!opcode_id || (opcode_id >= INSTRUCTION_SET_SIZE)) {
			fault_illegal_instruction();
			continue;
		}

		/* Evaluate the opcode structure and properly
		 * set the operands
		 */
		if (!opcode_oper2 && (instruction[opcode_id - 1].size > 1)) {
			/* Check if we're not out of RAM bondaries */
			if (!mm_grant_zone_normal(prip + (ARCH_ADDR_BITS >> (3 - !opcode_oper1))))
				continue;

			opcode_oper2 = ntohl(*((leg_addr_t *) &((char *) mm)[prip + (ARCH_ADDR_BITS >> (3 - !opcode_oper1))]));

			opcode_size += (ARCH_ADDR_BITS >> 3);

			operand2_type = OPERAND_TYPE_LIT;
		}

		if (!opcode_oper1 && (instruction[opcode_id - 1].size > 0)) {
			/* Check if we're not out of RAM bondaries */

			if (!mm_grant_zone_normal(prip + (ARCH_ADDR_BITS >> 3)))
				continue;

			opcode_oper1 = ntohl(*((leg_addr_t *) &((char *) mm)[prip + (ARCH_ADDR_BITS >> 3)]));

			opcode_size += (ARCH_ADDR_BITS >> 3);

			operand1_type = OPERAND_TYPE_LIT;
		}

		/* Process instruction */
		instruction[opcode_id - 1].doop(opcode_oper1, opcode_oper2, opcode_size, operand1_type, operand2_type);

		/* Check for hardware interrupts */
		interrupt_hw_check();
	}
}

