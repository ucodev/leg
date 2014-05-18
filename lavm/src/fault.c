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

#include "archdefs.h"
#include "fault.h"
#include "run.h"
#include "register.h"
#include "task.h"
#include "debug.h"
#include "vm.h"

void fault_mc(void) {
#ifdef DEBUG
	puts("CPU FAULT: Machine Check Fault.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	vm_destroy();
}

void fault_bad_mm(uint32_t addr) {
#ifdef DEBUG
	printf("CPU FAULT: Bad memory reference: 0x%.8X\n", addr);
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_BAD_MM;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_bad_reg(uint32_t regid) {
#ifdef DEBUG
	printf("CPU FAULT: Bad register ID: 0x%.8X\n", regid);
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_BAD_REG;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_bad_reg_val(uint32_t regid, uint32_t regval) {
#ifdef DEBUG
	printf("CPU FAULT: Bad register value. Register: 0x%.8X, Value: 0x%.8X\n", regid, regval);
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_BAD_REGVAL;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}


void fault_bad_oper_val(uint32_t value) {
#ifdef DEBUG
	printf("CPU FAULT: Bad operation value: 0x%.8X\n", value);
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_BAD_OPERVAL;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_illegal_interrupt(uint32_t intrid) {
#ifdef DEBUG
	printf("CPU FAULT: Illegal Interrupt. Interrupt ID: 0x%.8X\n", intrid);
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_ILL_INTR;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_illegal_instruction(void) {
#ifdef DEBUG
	puts("CPU FAULT: Illegal Instruction.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_ILL_OPCODE;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_fpu(void) {
#ifdef DEBUG
	puts("CPU FAULT: Floating Point Unit Fault.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_FPU;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_alu(void) {
#ifdef DEBUG
	puts("CPU FAULT: Arithmetic Logic Unit Fault.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_ALU;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_no_page(uint32_t laddr) {
#ifdef DEBUG
	puts("CPU FAULT: Page not found.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X, Logic Address: 0x%.8X\n", regs.rip, run.opcode, laddr);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_NO_PAGE;

	if (regs.rst & REG_RST_BIT_FH) {
		/* Fill rgp8 with laddr value, so we can keep track of the
		 * logical address.
		 */
		regs.rgp8 = laddr;

		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_page_perm(uint32_t laddr, uint32_t flags) {
#ifdef DEBUG
	puts("CPU FAULT: Permission denied on Logical Address.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X, Logical Address: 0x%.8X, Page Flags: 0x%.8X\n", regs.rip, run.opcode, laddr, flags);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_PAGE_PERM;

	if (regs.rst & REG_RST_BIT_FH) {
		/* Fill rgp8 with laddr and rgp7 with page flags */
		regs.rgp8 = laddr;
		regs.rgp7 = flags;

		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_no_priv(void) {
#ifdef DEBUG
	puts("CPU FAULT: Invalid Privilege Level for requested operation.");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_NO_PRIV;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

void fault_io_op(void) {
#ifdef DEBUG
	puts("CPU FAULT: Input/Output Operation Fault");
	printf("RIP: 0x%.8X, Opcode: 0x%.8X\n", regs.rip, run.opcode);
#endif

	/* Update RCT data if task bit is enabled on RST */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	regs.rff |= FAULT_IO_OP;

	if (regs.rst & REG_RST_BIT_FH) {
		regs.rip = regs.rfa;
		return;
	}

	exit(EXIT_FAILURE);
}

