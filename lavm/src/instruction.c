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
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "archdefs.h"
#include "register.h"
#include "instruction.h"
#include "interrupt.h"
#include "mm.h"
#include "io.h"
#include "fault.h"
#include "paging.h"
#include "task.h"
#include "privilege.h"
#include "config.h"
#include "debug.h"
#include "alu.h"
#include "fpu.h"


/* Instruction table */
const struct instruction instruction[INSTRUCTION_SET_SIZE] = {
	{ /* 0x01 */ 2, &cpvr }, /* Copy Reg	2 args: <Reg> <Ref>	*/
	{ /* 0x02 */ 2, &cpvl }, /* Copy Ltral	2 args: <Val> <Ref> 	*/
	{ /* 0x03 */ 2, &cpr },  /* Copy Ref	2 args: <Ref1> <Ref2>	*/
	{ /* 0x04 */ 2, &cprr }, /* Copy R-RR	2 args: <Reg> <RReg>	*/
	{ /* 0x05 */ 2, &cmp },  /* Compare	2 args: <Ref1> <Ref2>	*/
	{ /* 0x06 */ 1, &jmp },  /* Jump	1 arg:	<Addr>		*/
	{ /* 0x07 */ 1, &call }, /* Call	1 arg:	<Addr>		*/
	{ /* 0x08 */ 0, &ret },  /* Return	0 args			*/
	{ /* 0x09 */ 2, &arth }, /* Arithmetic	2 args: <Ref1> <Ref2>	*/
	{ /* 0x0A */ 2, &lgic }, /* Logic	2 args: <Ref1> <Ref2>	*/
	{ /* 0x0B */ 1, &intr }, /* Interrupt	1 arg:	<Interrupt ID>	*/
	{ /* 0x0C */ 2, &ceb },  /* Real-time	1 arg:	<Nr Bytes>	*/
	{ /* 0x0D */ 0, &nop },  /* No Oper	0 args			*/
	{ /* 0x0E */ 0, &ltsk }, /* Load Task	0 args			*/
};

static volatile void *ref_decode(leg_addr_t ref, uint8_t operand_type) {
	return (operand_type == OPERAND_TYPE_REG) ? register_decode(ref) : (void *) (mm + ref);
}

static int operand_isreg(uint8_t operand_type) {
	return (operand_type == OPERAND_TYPE_REG);
}

static int operand_islit(uint8_t operand_type) {
	return (operand_type == OPERAND_TYPE_LIT);
}

void cpvr(	leg_addr_t reg,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	leg_addr_t dso = reg, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* Grant that source operand is a register */
	if (operand_isreg(sond_type)) {
		/* Grant that source operand is a _valid_ register */
		if (!register_grant_any(reg))
			return; // Restart instruction

		/* Evaluate privilege level for read access on source operand */
		if (!privilege_reg_req_read(privilege_get_current(), reg))
			return;	// Restart instruction
	} else {
		fault_illegal_instruction();
		return;	// Restart instruction
	}

	/* If target operand is a register ... */
	if (operand_isreg(tond_type)) {
		/* Grant that target operand is a _valid_ register */
		if (!register_grant_any(to))
			return; // Restart instruction

		/* Verify the privilege level for register access */
		if (!privilege_reg_req_write(privilege_get_current(), to))
			return; // Restart instruction
	} else {
		/* If target operand is a memory reference ... */

		/* Check if paging is enabled ... */
		if (regs.rst & REG_RST_BIT_PAGING) {
			/* And translate Logical to Physical address and grant
			 * that the page has Read/Write privileges.
			 */
			if (!(to = paging_get_paddr(to, PAGE_PERM_RW)))
				return; // Restart instruction
		}

		/* Sanity check - Validate zone access and boundaries */
		if (!mm_grant_zone_normal(to))
			return; // Restart instruction
	}

	/* Copy value from source operand to target operand with correct byte
	 * order convertion (LEG is big endian, x86/x64 is little endian).
	 */
	*(leg_addr_t *) ref_decode(to, tond_type) = operand_islit(tond_type) ? htonl(*(leg_addr_t *) ref_decode(reg, sond_type)) : *(leg_addr_t *) ref_decode(reg, sond_type);

	/* Update RIP to point to the next instruction */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
	
}

void cpvl(	leg_addr_t val,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	leg_addr_t dso = val, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* If source operand isn't a literal ... */
	if (sond_type != OPERAND_TYPE_LIT) {
		fault_illegal_instruction();
		return;	// Restart instruction
	}

	/* If target operand is a register... */
	if (operand_isreg(tond_type)) {
		/* Grant that target operand is a _valid_ register */
		if (!register_grant_any(to))
			return; // Restart instruction

		/* Verify the privilege level for register access */
		if (!privilege_reg_req_write(privilege_get_current(), to))
			return;	// Restart instruction
	} else {
		/* If target operand is a memory reference ... */

		/* Check if paging is enabled ... */
		if (regs.rst & REG_RST_BIT_PAGING) {
			/* And translate Logical to Physical address and grant
			 * that the page has Read/Write privileges.
			 */
			if (!(to = paging_get_paddr(to, PAGE_PERM_RW)))
				return; // Restart instruction
		}

		/* Sanity check - Validate zone access and boundaries */
		if (!mm_grant_zone_normal(to))
			return; // Restart instruction
	}

	/* Copy value from source operand to target operand with correct byte
	 * order convertion (LEG is big endian, x86/x64 is little endian).
	 */
	*(leg_addr_t *) ref_decode(to, tond_type) = operand_islit(tond_type) ? htonl(val) : val;

	/* Update RIP to point to the next instruction */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void cpr(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	leg_addr_t dso = from, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* If source operand is a register... */
	if (operand_isreg(sond_type)) {
		/* Grant that source operand is a register */
		if (!register_grant_any(from))
			return; // Restart instruction

		/* Verify the privilege level for register access */
		if (!privilege_reg_req_read(privilege_get_current(), from))
			return;	// Restart instruction
	} else {
		/* If source operand is a memory reference ... */

		/* Check if paging is enabled ... */
		if (regs.rst & REG_RST_BIT_PAGING) {
			/* And translate Logical to Physical address and grant
			 * that the page has Read/Write privileges.
			 */
			if (!(from = paging_get_paddr(from,
					PAGE_PERM_RO | PAGE_PERM_RW)))
				return; // Restart instruction
		}

		/* Sanity check - Validate zone access and boundaries */
		if (!mm_grant_zone_normal(from))
			return; // Restart instruction
	}

	/* If target operand is a register... */
	if (operand_isreg(tond_type)) {
		/* Grant that target operand is a register */
		if (!register_grant_any(to))
			return; // Restart instruction

		/* Verify the privilege level for register access */
		if (!privilege_reg_req_read(privilege_get_current(), to))
			return;	// Restart instruction
	} else {
		/* If target operand is a memory reference ... */

		/* Check if paging is enabled ... */
		if (regs.rst & REG_RST_BIT_PAGING) {
			/* And translate Logical to Physical address and grant
			 * that the page has Read/Write privileges.
			 */
			if (!(to = paging_get_paddr(to, PAGE_PERM_RW)))
				return; // Restart instruction
		}

		/* Sanity check - Validate zone access and boundaries */
		if (!mm_grant_zone_normal(to))
			return; // Restart instruction
	}

	if (operand_islit(tond_type) && operand_isreg(sond_type)) {
		/* Sanity check */
		if (!mm_grant_zone_normal(*(leg_addr_t *) ref_decode(from, sond_type)))
			return;

		/* Copy memory (register value is an address) to memory,
		 * no byte reorder needed.
		 */
		*(leg_addr_t *) ref_decode(to, tond_type) = *(leg_addr_t *) (mm + *(leg_addr_t *) ref_decode(from, sond_type));
	} else if (operand_isreg(tond_type) && operand_islit(sond_type)) {
		/* Copy memory to register, big to little endian */
		*(leg_addr_t *) ref_decode(to, tond_type) = ntohl(*(leg_addr_t *) ref_decode(from, sond_type));
	} else if (operand_isreg(tond_type) && operand_isreg(sond_type)) {
		/* Sanity check */
		if (!mm_grant_zone_normal(*(leg_addr_t *) ref_decode(from, sond_type)))
			return;

		/* Copy memory (register value is an address) to register,
		 * big to little endian
		 */
		*(leg_addr_t *) ref_decode(to, tond_type) = ntohl(*(leg_addr_t *) (mm + *(leg_addr_t *) ref_decode(from, sond_type)));
	} else {
		/* Copy memory to memory, no byte reordering */
		*(leg_addr_t *) ref_decode(to, tond_type) = *(leg_addr_t *) ref_decode(from, sond_type);
	}

	/* Update RIP to point to the next instruction */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void cprr(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_addr_t to_ref;

#ifdef DEBUG
	leg_addr_t dso = from, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* Grant that source operand is a register */
	if (!operand_isreg(sond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that target operand is a register */
	if (!operand_isreg(tond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that source operand is a _valid_ register */
	if (!register_grant_any(from))
		return; // Restart instruction

	/* Grant that target operand is a _valid_ register */
	if (!register_grant_any(to))
		return; // Restart instruction

	/* Grant that there's read permission on source operand */
	if (!privilege_reg_req_write(privilege_get_current(), to))
		return; // Restart instruction

	/* Grant that there's write permission on target operand */
	if (!privilege_reg_req_read(privilege_get_current(), from))
		return; // Restart instruction

	/* Extract memory reference from target operand */
	to_ref = (*(leg_addr_t *) ref_decode(to, tond_type));

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical to Physical address and grant that
		 * there Read/Write privileges on that page
		 */
		if (!(to_ref = paging_get_paddr(to_ref, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Grant zone normal */
	if (!mm_grant_zone_normal(to_ref))
		return; // Restart instruction

	/* Copy source operand value to the memory reference pointed by the
	 * target operand
	 */
	*(leg_addr_t *) (mm + to_ref) = htonl(*(leg_addr_t *) ref_decode(from, sond_type));

	/* Update RIP to point to the next instruction */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void cmp(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_addr_t val1, val2;

#ifdef DEBUG
	leg_addr_t dso = from, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* Grant that source operand is a register */
	if (!operand_isreg(sond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that target operand is a register */
	if (!operand_isreg(tond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that source operand is a _valid_ register */
	if (!register_grant_any(from))
		return; // Restart instruction

	/* Grant that target operand is a _valid_ register */
	if (!register_grant_any(to))
		return; // Restart instruction

	/* Grant that there's read permission on source operand */
	if (!privilege_reg_req_write(privilege_get_current(), to))
		return; // Restart instruction

	/* Grant that there's write permission on target operand */
	if (!privilege_reg_req_read(privilege_get_current(), from))
		return; // Restart instruction

	/* Clear result bit from RCMP */
	regs.rcmp &= ~(1 << (REG_RCMP_BIT_RESULT - 1));

	/* Extract values from registers */
	val1 = *(leg_addr_t *) ref_decode(from, sond_type);
	val2 = *(leg_addr_t *) ref_decode(to, tond_type);

	/* Perform comparison */
	switch (regs.rcmp & 
			(REG_RCMP_BIT_NE | REG_RCMP_BIT_GT |
			REG_RCMP_BIT_LT | REG_RCMP_BIT_EQ)) {

		case REG_RCMP_BIT_NE:
			regs.rcmp |= ((val1 != val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;
		case REG_RCMP_BIT_GT:
			regs.rcmp |= ((val1 > val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;
		case REG_RCMP_BIT_LT:
			regs.rcmp |= ((val1 < val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;
		case REG_RCMP_BIT_EQ:
			regs.rcmp |= ((val1 == val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;
		case REG_RCMP_BITS_GE:
			regs.rcmp |= ((val1 >= val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;
		case REG_RCMP_BITS_LE:
			regs.rcmp |= ((val1 <= val2) <<
					(REG_RCMP_BIT_RESULT - 1));
			break;

		default: fault_bad_reg_val(REG_RCMP, regs.rcmp);
			 return; // Restart instruction
	}

	/* Update RIP to point to the next instruction */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void jmp(	leg_addr_t addr,
		leg_addr_t reserved,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	leg_addr_t dso = addr;

	if (!debug_instruction_enter(__func__, dso, 0, opcode_size,
			sond_type, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	/* If RCMP result bit is 1, jump */
	if (regs.rcmp & REG_RCMP_BIT_RESULT)
		regs.rip = addr;
	else
		regs.rip += opcode_size;

	//regs.rip = regs.rcmp & REG_RCMP_BIT_RESULT ? addr : regs.rip += opcode_size, regs.rip;


#ifdef DEBUG
	debug_instruction_leave(__func__, dso, 0, opcode_size,
		sond_type, OPERAND_TYPE_NONE);
#endif
}

void call(	leg_addr_t addr,
		leg_addr_t reserved,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_addr_t prra = regs.rra; // Assume RRA points to a Physical Address

#ifdef DEBUG
	leg_addr_t dso = addr;

	if (!debug_instruction_enter(__func__, dso, 0, opcode_size,
			sond_type, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	/* Increment temporary RRA address */
	prra += (ARCH_ADDR_BITS >> 3);

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prra = paging_get_paddr(prra, PAGE_PERM_RO)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RRA Address belongs to normal zone */
	if (!mm_grant_zone_normal(prra))
		return; // Restart instruction

	/* Push return address into RRA */
	*(leg_addr_t *) (mm + prra) = htonl(regs.rip + opcode_size);

	/* Update RRA value */
	regs.rra += (ARCH_ADDR_BITS >> 3);

	/* Jump to (Call) routine */
	regs.rip = addr;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, 0, opcode_size,
		sond_type, OPERAND_TYPE_NONE);
#endif
}

void ret(	leg_addr_t reserved1,
		leg_addr_t reserved2,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_addr_t prra = regs.rra; // Assume RRA points to a Physical Address

#ifdef DEBUG
	if (!debug_instruction_enter(__func__, 0, 0, opcode_size,
			OPERAND_TYPE_NONE, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prra = paging_get_paddr(prra, PAGE_PERM_RO)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RRA Address belongs to normal zone */
	if (!mm_grant_zone_normal(prra))
		return;

	/* Pop return address */
	regs.rip = ntohl(*(leg_addr_t *) (mm + prra));

	/* Update RRA value */
	regs.rra -= (ARCH_ADDR_BITS >> 3);

#ifdef DEBUG
	debug_instruction_leave(__func__, 0, 0, opcode_size,
		OPERAND_TYPE_NONE, OPERAND_TYPE_NONE);
#endif
}

void arth(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_char_t char_val1 = 0, char_val2 = 0;
	leg_uchar_t uchar_val1 = 0, uchar_val2 = 0;
	leg_short_t short_val1 = 0, short_val2 = 0;
	leg_ushort_t ushort_val1 = 0, ushort_val2 = 0;
	leg_int_t int_val1 = 0, int_val2 = 0;
	leg_uint_t uint_val1 = 0, uint_val2 = 0;
	leg_ulong_t ulong_val1 = 0, ulong_val2 = 0;
	leg_long_t long_val1 = 0, long_val2 = 0;
	leg_float_t float_val1 = 0, float_val2 = 0;
	leg_double_t double_val = 0;
	uint8_t type = 0;

	/* TODO: break this huge function into smaller routines */
#ifdef DEBUG
	leg_addr_t dso = from, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* Grant that source operand is a register */
	if (!operand_isreg(sond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that target operand is a register */
	if (!operand_isreg(tond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that source operand is a _valid_ register */
	if (!register_grant_any(from))
		return; // Restart instruction

	/* Grant that target operand is a _valid_ register */
	if (!register_grant_any(to))
		return; // Restart instruction

	/* Grant that there's read permission on source operand */
	if (!privilege_reg_req_write(privilege_get_current(), to))
		return; // Restart instruction

	/* Grant that there's write permission on target operand */
	if (!privilege_reg_req_read(privilege_get_current(), from))
		return; // Restart instruction

	/* Get values from registers into temporary variables */
	if (register_is_rfp(from) || register_is_rfp(to)) {
		memcpy(&float_val1, (void *) ref_decode(from, sond_type), 4);
		memcpy(&float_val2, (void *) ref_decode(to, tond_type), 4);

		/* Extended operand indicates double precision */
		type = 1 + !!(regs.rarth & (REG_ARTH_BIT_ERFP1 | REG_ARTH_BIT_ERFP2 | REG_ARTH_BIT_ERFP3 | REG_ARTH_BIT_ERFP4));
	} else {
		if (regs.rarth & REG_ARTH_BIT_SIGNED) {
			if (regs.rarth & REG_ARTH_BIT_8BITOP) {
				char_val1 = *(leg_char_t *) ref_decode(from, sond_type);
				char_val2 = *(leg_char_t *) ref_decode(to, tond_type);
				type = 5;
			} else if (regs.rarth & REG_ARTH_BIT_16BITOP) {
				short_val1 = *(leg_short_t *) ref_decode(from, sond_type);
				short_val2 = *(leg_short_t *) ref_decode(to, tond_type);
				type = 6;
			} else if (regs.rarth & REG_ARTH_BIT_32BITOP) {
#if ARCH_ADDR_BITS == 64
				int_val1 = *(leg_int_t *) ref_decode(from, sond_type);
				int_val2 = *(leg_int_t *) ref_decode(to, tond_type);
				type = 7;
#else
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
#endif
			} else {
				long_val1 = *(leg_long_t *) ref_decode(from, sond_type);
				long_val2 = *(leg_long_t *) ref_decode(to, tond_type);
				type = 3;
			}
		} else {
			if (regs.rarth & REG_ARTH_BIT_8BITOP) {
				uchar_val1 = *(leg_uchar_t *) ref_decode(from, sond_type);
				uchar_val2 = *(leg_uchar_t *) ref_decode(to, tond_type);
				type = 8;
			} else if (regs.rarth & REG_ARTH_BIT_16BITOP) {
				ushort_val1 = *(leg_ushort_t *) ref_decode(from, sond_type);
				ushort_val2 = *(leg_ushort_t *) ref_decode(to, tond_type);
				type = 9;
			} else if (regs.rarth & REG_ARTH_BIT_32BITOP) {
#if ARCH_ADDR_BITS == 64
				uint_val1 = *(leg_uint_t *) ref_decode(from, sond_type);
				uint_val2 = *(leg_uint_t *) ref_decode(to, tond_type);
				type = 10;
#else
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
#endif
			} else {
				ulong_val1 = *(leg_ulong_t *) ref_decode(from, sond_type);
				ulong_val2 = *(leg_ulong_t *) ref_decode(to, tond_type);
				type = 4;
			}
		}
	}

	/* Remove OF and UF bits if they're set */
	regs.rarth &= ~REG_ARTH_BIT_OF;
	regs.rarth &= ~REG_ARTH_BIT_UF;

	/* Perform arithmetic operation */
	switch (regs.rarth &
			(REG_ARTH_BIT_MUL | REG_ARTH_BIT_DIV |
			REG_ARTH_BIT_SUB | REG_ARTH_BIT_ADD |
			REG_ARTH_BIT_MOD)) {

		case REG_ARTH_BIT_MUL:
			switch (type) {
				case 1: float_val2 = fpu_mul_float(float_val1, float_val2); break;
				case 2: double_val = fpu_mul_double(float_val1, float_val2); break;
				case 3: long_val2 = alu_mul_long(long_val1, long_val2); break;
				case 4: ulong_val2 = alu_mul_ulong(ulong_val1, ulong_val2);
				case 5: char_val2 = alu_mul_char(char_val1, char_val2); break;
				case 6: short_val2 = alu_mul_short(short_val1, short_val2); break;
				case 7: int_val2 = alu_mul_int(int_val1, int_val2); break;
				case 8: uchar_val2 = alu_mul_uchar(uchar_val1, uchar_val2); break;
				case 9: ushort_val2 = alu_mul_ushort(ushort_val1, ushort_val2); break;
				case 10: uint_val2 = alu_mul_uint(uint_val1, uint_val2); break;
				default: fault_bad_reg_val(REG_RARTH, regs.rarth); return;
			}

			break;
		case REG_ARTH_BIT_DIV:
			if (type == 1) {
				if (!float_val1) {
					fault_fpu();
					return;
				}

				float_val2 = fpu_div_float(float_val1, float_val2);
			} else if (type == 2) {
				if (!float_val1) {
					fault_fpu();
					return;
				}

				double_val = fpu_div_double(float_val1, float_val2);
			} else if (type == 3) {
				if (!long_val1) {
					fault_alu();
					return;
				}

				long_val2 = alu_div_long(long_val1, long_val2);
			} else if (type == 4) {
				if (!ulong_val1) {
					fault_alu();
					return;
				}

				ulong_val2 = alu_div_ulong(ulong_val1, ulong_val2);
			} else if (type == 5) {
				if (!char_val1) {
					fault_alu();
					return;
				}

				char_val2 = alu_div_char(char_val1, char_val2);
			} else if (type == 6) {
				if (!short_val1) {
					fault_alu();
					return;
				}

				short_val2 = alu_div_short(short_val1, short_val2);
			} else if (type == 7) {
				if (!int_val1) {
					fault_alu();
					return;
				}

				int_val2 = alu_div_int(int_val1, int_val2);
			} else if (type == 8) {
				if (!uchar_val1) {
					fault_alu();
					return;
				}

				uchar_val2 = alu_div_uchar(uchar_val1, uchar_val2);
			} else if (type == 9) {
				if (!ushort_val1) {
					fault_alu();
					return;
				}

				ushort_val2 = alu_div_ushort(ushort_val1, ushort_val2);
			} else if (type == 10) {
				if (!uint_val1) {
					fault_alu();
					return;
				}

				uint_val2 = alu_div_uint(uint_val1, uint_val2);
			} else {
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
			}

			break;
		case REG_ARTH_BIT_SUB:
			switch (type) {
				case 1: float_val2 = fpu_sub_float(float_val1, float_val2); break;
				case 2: double_val = fpu_sub_double(float_val1, float_val2); break;
				case 3: long_val2 = alu_sub_long(long_val1, long_val2); break;
				case 4: ulong_val2 = alu_sub_ulong(ulong_val1, ulong_val2);
				case 5: char_val2 = alu_sub_char(char_val1, char_val2); break;
				case 6: short_val2 = alu_sub_short(short_val1, short_val2); break;
				case 7: int_val2 = alu_sub_int(int_val1, int_val2); break;
				case 8: uchar_val2 = alu_sub_uchar(uchar_val1, uchar_val2); break;
				case 9: ushort_val2 = alu_sub_ushort(ushort_val1, ushort_val2); break;
				case 10: uint_val2 = alu_sub_uint(uint_val1, uint_val2); break;
				default: fault_bad_reg_val(REG_RARTH, regs.rarth); return;
			}
			break;
		case REG_ARTH_BIT_ADD:
			switch (type) {
				case 1: float_val2 = fpu_add_float(float_val1, float_val2); break;
				case 2: double_val = fpu_add_double(float_val1, float_val2); break;
				case 3: long_val2 = alu_add_long(long_val1, long_val2); break;
				case 4: ulong_val2 = alu_add_ulong(ulong_val1, ulong_val2);
				case 5: char_val2 = alu_add_char(char_val1, char_val2); break;
				case 6: short_val2 = alu_add_short(short_val1, short_val2); break;
				case 7: int_val2 = alu_add_int(int_val1, int_val2); break;
				case 8: uchar_val2 = alu_add_uchar(uchar_val1, uchar_val2); break;
				case 9: ushort_val2 = alu_add_ushort(ushort_val1, ushort_val2); break;
				case 10: uint_val2 = alu_add_uint(uint_val1, uint_val2); break;
				default: fault_bad_reg_val(REG_RARTH, regs.rarth); return;
			}
			break;
		case REG_ARTH_BIT_MOD:
			if (type == 1) {
				/* No mod op permitted on RFP regs */
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
			} else if (type == 2) {
				/* No mod op permitted on RFP regs */
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
			} else if (type == 3) {
				if (!long_val1) {
					fault_alu();
					return;
				}

				long_val2 = alu_mod_long(long_val1, long_val2);
			} else if (type == 4) {
				if (!ulong_val1) {
					fault_alu();
					return;
				}

				ulong_val2 = alu_mod_ulong(ulong_val1, ulong_val2);
			} else if (type == 5) {
				if (!char_val1) {
					fault_alu();
					return;
				}

				char_val2 = alu_mod_char(char_val1, char_val2);
			} else if (type == 6) {
				if (!short_val1) {
					fault_alu();
					return;
				}

				short_val2 = alu_mod_short(short_val1, short_val2);
			} else if (type == 7) {
				if (!int_val1) {
					fault_alu();
					return;
				}

				int_val2 = alu_mod_int(int_val1, int_val2);
			} else if (type == 8) {
				if (!uchar_val1) {
					fault_alu();
					return;
				}

				uchar_val2 = alu_mod_uchar(uchar_val1, uchar_val2);
			} else if (type == 9) {
				if (!ushort_val1) {
					fault_alu();
					return;
				}

				ushort_val2 = alu_mod_ushort(ushort_val1, ushort_val2);
			} else if (type == 10) {
				if (!uint_val1) {
					fault_alu();
					return;
				}

				uint_val2 = alu_mod_uint(uint_val1, uint_val2);
			} else {
				fault_bad_reg_val(REG_RARTH, regs.rarth);
				return;
			}

			break;
		default: fault_bad_reg_val(REG_RARTH, regs.rarth);
			 return; // Restart instruction
	}

	/* Update target operand with the result */
	if (type == 1) {
		memcpy((void *) ref_decode(to, tond_type), &float_val2, 4);
	} else if (type == 2) {
		memcpy((void *) ref_decode(to, tond_type), &double_val, 4);
		/* Most significant part is stored at the selected extended
		 * operand register
		 */
	} else if (type == 3) {
		*(leg_long_t *) ref_decode(to, tond_type) = long_val2;
	} else if (type == 4) {
		*(leg_ulong_t *) ref_decode(to, tond_type) = ulong_val2;
	} else if (type == 5) {
		*(leg_char_t *) ref_decode(to, tond_type) = char_val2;
	} else if (type == 6) {
		*(leg_short_t *) ref_decode(to, tond_type) = short_val2;
	} else if (type == 7) {
		*(leg_int_t *) ref_decode(to, tond_type) = int_val2;
	} else if (type == 8) {
		*(leg_uchar_t *) ref_decode(to, tond_type) = uchar_val2;
	} else if (type == 9) {
		*(leg_ushort_t *) ref_decode(to, tond_type) = ushort_val2;
	} else if (type == 10) {
		*(leg_uint_t *) ref_decode(to, tond_type) = uint_val2;
	} else {
		fault_bad_reg_val(REG_RARTH, regs.rarth);
		return;
	}

	/* Update RIP */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void lgic(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	leg_uchar_t uchar_val1 = 0, uchar_val2 = 0;
	leg_ushort_t ushort_val1 = 0, ushort_val2 = 0;
	leg_uint_t uint_val1 = 0, uint_val2 = 0;
	leg_ulong_t ulong_val1 = 0, ulong_val2 = 0;
	leg_addr_t trlgic = regs.rlgic;
	uint8_t neg_res = 0, type = 0;

	/* TODO: break this huge function into smaller routines */
#ifdef DEBUG
	leg_addr_t dso = from, dto = to;

	if (!debug_instruction_enter(__func__, dso, dto, opcode_size,
			sond_type, tond_type))
		return; // Incomplete. Restart instruction.
#endif

	/* Grant that source operand is a register */
	if (!operand_isreg(sond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that target operand is a register */
	if (!operand_isreg(tond_type)) {
		fault_illegal_instruction();
		return; // Restart instruction
	}

	/* Grant that source operand is a _valid_ register */
	if (!register_grant_any(from))
		return; // Restart instruction

	/* Grant that target operand is a _valid_ register */
	if (!register_grant_any(to))
		return; // Restart instruction

	/* No lgic instructions of RFP registers */
	if (register_is_rfp(to) || register_is_rfp(from)) {
		fault_bad_reg(to);
		return;
	}

	/* Grant that there's read permission on source operand */
	if (!privilege_reg_req_write(privilege_get_current(), to))
		return; // Restart instruction

	/* Grant that there's write permission on target operand */
	if (!privilege_reg_req_read(privilege_get_current(), from))
		return; // Restart instruction

	if (regs.rlgic & REG_RLGIC_BIT_8BITOP) {
		uchar_val1 = *(leg_uchar_t *) ref_decode(from, sond_type);
		uchar_val2 = *(leg_uchar_t *) ref_decode(to, tond_type);
		type = 1;
	} else if (regs.rlgic & REG_RLGIC_BIT_16BITOP) {
		ushort_val1 = *(leg_ushort_t *) ref_decode(from, sond_type);
		ushort_val2 = *(leg_ushort_t *) ref_decode(to, tond_type);
		type = 2;
	} else if (regs.rlgic & REG_RLGIC_BIT_32BITOP) {
#if ARCH_ADDR_BITS == 64
		uint_val1 = *(leg_uint_t *) ref_decode(from, sond_type);
		uint_val2 = *(leg_uint_t *) ref_decode(to, tond_type);
		type = 3;
#else
		fault_bad_reg_val(REG_RLGIC, regs.rlgic);
#endif
	} else {
		ulong_val1 = *(leg_ulong_t *) ref_decode(from, sond_type);
		ulong_val2 = *(leg_ulong_t *) ref_decode(to, tond_type);
		type = 4;
	}

	/* Check if there are multiple (and valid) operations */
	if ((regs.rlgic & ~REG_RLGIC_BIT_NOT) &&
			(regs.rlgic & REG_RLGIC_BIT_NOT)) {
		trlgic = regs.rlgic & ~REG_RLGIC_BIT_NOT;
		neg_res = 1;
	}

	switch (trlgic &
			(REG_RLGIC_BIT_NOT | REG_RLGIC_BIT_XOR |
			REG_RLGIC_BIT_AND | REG_RLGIC_BIT_OR |
			REG_RLGIC_BIT_SHL | REG_RLGIC_BIT_SHR |
			REG_RLGIC_BIT_ROL)) {

		case REG_RLGIC_BIT_NOT: 
			switch (type) {
				case 1: uchar_val2 = alu_not_uchar(uchar_val1); break;
				case 2: ushort_val2 = alu_not_ushort(ushort_val1); break;
				case 3: uint_val2 = alu_not_uint(uint_val1); break;
				case 4: ulong_val2 = alu_not_ulong(ulong_val1); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_XOR:
			switch (type) {
				case 1: uchar_val2 = alu_xor_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_xor_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_xor_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_xor_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_AND:
			switch (type) {
				case 1: uchar_val2 = alu_and_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_and_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_and_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_and_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_OR:
			switch (type) {
				case 1: uchar_val2 = alu_or_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_or_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_or_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_or_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_SHL:
			switch (type) {
				case 1: uchar_val2 = alu_shl_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_shl_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_shl_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_shl_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_SHR:
			switch (type) {
				case 1: uchar_val2 = alu_shr_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_shr_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_shr_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_shr_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_ROL:
			switch (type) {
				case 1: uchar_val2 = alu_rol_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_rol_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_rol_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_rol_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		case REG_RLGIC_BIT_ROR:
			switch (type) {
				case 1: uchar_val2 = alu_ror_uchar(uchar_val1, uchar_val2); break;
				case 2: ushort_val2 = alu_ror_ushort(ushort_val1, ushort_val2); break;
				case 3: uint_val2 = alu_ror_uint(uint_val1, uint_val2); break;
				case 4: ulong_val2 = alu_ror_ulong(ulong_val1, ulong_val2); break;
				default: fault_bad_reg_val(REG_RLGIC, regs.rlgic); return;
			}
			break;
		default: fault_bad_reg_val(REG_RLGIC, regs.rlgic);
			 return; // Restart instruction
	}

	/* Update target operand with the result */
	if (type == 1) {
		/* Perform a NOT after the logic op if necessary */
		if (neg_res)
			uchar_val2 = alu_not_uchar(uchar_val2);

		*(leg_uchar_t *) ref_decode(to, tond_type) = uchar_val2;
	} else if (type == 2) {
		/* Perform a NOT after the logic op if necessary */
		if (neg_res)
			ushort_val2 = alu_not_ushort(ushort_val2);

		*(leg_ushort_t *) ref_decode(to, tond_type) = ushort_val2;
	} else if (type == 3) {
		/* Perform a NOT after the logic op if necessary */
		if (neg_res)
			uint_val2 = alu_not_uint(uint_val2);

		*(leg_uint_t *) ref_decode(to, tond_type) = uint_val2;
	} else if (type == 4) {
		/* Perform a NOT after the logic op if necessary */
		if (neg_res)
			ulong_val2 = alu_not_ulong(ulong_val2);

		*(leg_ulong_t *) ref_decode(to, tond_type) = ulong_val2;
	} else {
		fault_bad_reg_val(REG_RLGIC, regs.rlgic);
		return;
	}
		
	/* Update RIP */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, dto, opcode_size,
		sond_type, tond_type);
#endif
}

void intr(	leg_addr_t intrid,
		leg_addr_t reserved,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	leg_addr_t dso = intrid;

	if (!debug_instruction_enter(__func__, dso, 0, opcode_size,
			OPERAND_TYPE_LIT, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	switch (intrid) {
		case INTR_01:
			interrupt_int01(regs.rgp1, regs.rgp2, regs.rgp3);
			break;
		case INTR_03:
			interrupt_int03();
			break;
		case INTR_0A:
			interrupt_int0a(regs.rgp1);
			break;
		case INTR_0B:
			interrupt_int0b(regs.rgp1, regs.rgp2, regs.rgp3, regs.rgp4, regs.rgp5);
			break;
		case INTR_0D:
			interrupt_int0d(regs.rgp1);
			break;
		default: interrupt_intvr_handler(intrid);
	}

	/* NOTE: RIP is updated inside each interrupt_int*() function */

#ifdef DEBUG
	debug_instruction_leave(__func__, dso, 0, opcode_size,
		OPERAND_TYPE_LIT, OPERAND_TYPE_NONE);
#endif
}

void ceb(	leg_addr_t from,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	if (!debug_instruction_enter(__func__, 0, 0, opcode_size,
			OPERAND_TYPE_NONE, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif
	if ((to <= from) || ((to - from) % 4)) {
		fault_illegal_instruction();
		return;
	}

	/* CEB is not implemented on VM */

	/* Update RIP */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, 0, 0, opcode_size,
		OPERAND_TYPE_NONE, OPERAND_TYPE_NONE);
#endif
}

void nop(	leg_addr_t reserved,
		leg_addr_t reserved2,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#ifdef DEBUG
	if (!debug_instruction_enter(__func__, 0, 0, opcode_size,
			OPERAND_TYPE_NONE, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	/* Update RIP */
	regs.rip += opcode_size;

#ifdef DEBUG
	debug_instruction_leave(__func__, 0, 0, opcode_size,
		OPERAND_TYPE_NONE, OPERAND_TYPE_NONE);
#endif
}

void ltsk(	leg_addr_t reserved1,
		leg_addr_t reserved2,
		uint8_t opcode_size,
		uint8_t operand1_type,
		uint8_t operand2_type) {
	uint32_t prct = regs.rct; // Assume RCT value is a Physical Address

#ifdef DEBUG
	if (!debug_instruction_enter(__func__, 0, 0, opcode_size,
			OPERAND_TYPE_NONE, OPERAND_TYPE_NONE))
		return; // Incomplete. Restart instruction.
#endif

	/* If paging is enabled ... */
	if (regs.rst & REG_RST_BIT_PAGING) {
		/* Translate Logical Address to Physical Address */
		if (!(prct = paging_get_paddr(prct, PAGE_PERM_RW)))
			return; // Restart instruction
	}

	/* Check if temporary Physical RST Address belongs to normal zone */
	if (!mm_grant_zone_normal(prct))
		return;

	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rbt();
		task_load_rct();
	} else {
		/* This instruction has no effect is RST bit 2 is cleared */
		regs.rip += opcode_size;
	}

#ifdef DEBUG
	debug_instruction_leave(__func__, 0, 0, opcode_size,
		OPERAND_TYPE_NONE, OPERAND_TYPE_NONE);
#endif
}

