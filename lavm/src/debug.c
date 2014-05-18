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
#include <string.h>
#include <stdint.h>

#include <arpa/inet.h>

#include "archdefs.h"
#include "config.h"
#include "register.h"
#include "mm.h"
#include "paging.h"
#include "debug.h"


volatile char *const debug_operand_to_str(leg_addr_t operand, uint8_t type) {
	volatile static char name[128] = "<undefined>";

	if (type == OPERAND_TYPE_REG) {
		switch (operand) {
			case REG_RIP: return "rip"; break;
			case REG_RST: return "rst"; break;
			case REG_RFF: return "rff"; break;
			case REG_RFA: return "rfa"; break;
			case REG_RBT: return "rbt"; break;
			case REG_RCT: return "rct"; break;
			case REG_RPA: return "rpa"; break;
			case REG_RSA: return "rsa"; break;
			case REG_RRA: return "rra"; break;
			case REG_RCMP: return "rcmp"; break;
			case REG_RLGIC: return "rlgic"; break;
			case REG_RARTH: return "rarth"; break;
			case REG_RGP1: return "rgp1"; break;
			case REG_RGP2: return "rgp2"; break;
			case REG_RGP3: return "rgp3"; break;
			case REG_RGP4: return "rgp4"; break;
			case REG_RGP5: return "rgp5"; break;
			case REG_RGP6: return "rgp6"; break;
			case REG_RGP7: return "rgp7"; break;
			case REG_RGP8: return "rgp8"; break;
			case REG_RAL1: return "ral1"; break;
			case REG_RAL2: return "ral2"; break;
			case REG_RAL3: return "ral3"; break;
			case REG_RAL4: return "ral4"; break;
			case REG_RFP1: return "rfp1"; break;
			case REG_RFP2: return "rfp2"; break;
			case REG_RFP3: return "rfp3"; break;
			case REG_RFP4: return "rfp4"; break;
		}
	} else if (type == OPERAND_TYPE_LIT) {
		sprintf((char *) name, "0x%.8X", operand);
	}

	return name;
}

int debug_instruction_enter(
		const char *mnemonic,
		leg_addr_t so,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
	char dbgstr_partial[128] = { 0 }, dbgstr_final[512] = { 0 };

	/* Mnemonic and respective operands */
	if ((sond_type != OPERAND_TYPE_NONE) &&
			(tond_type != OPERAND_TYPE_NONE)) {
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
					" --> 0x%.8X: %s %s, %s",
					regs.rip,
					mnemonic,
					debug_operand_to_str(so, sond_type),
					debug_operand_to_str(to, tond_type));
	} else if ((sond_type != OPERAND_TYPE_NONE) &&
			(tond_type == OPERAND_TYPE_NONE)) {
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
					" --> 0x%.8X: %s %s",
					regs.rip,
					mnemonic,
					debug_operand_to_str(so, sond_type));
	} else {
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
					" --> 0x%.8X: %s",
					regs.rip,
					mnemonic);
	}

	strcpy(dbgstr_final, dbgstr_partial);

#if DEBUG_ENTER_OPERAND_STATUS == 1
	strncat(dbgstr_final, "\t# Enter(", 
		(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));

	/* Source Operand content */
	if (sond_type == OPERAND_TYPE_REG) {
		/* Grant that this is a register */
		if (!register_grant_any(so)) {
			printf("%s ... <incomplete>)\n", dbgstr_final);

			return 0;
		}

		/* Operand is a register */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: %s = 0x%.8X",
				debug_operand_to_str(so, sond_type),
				*(leg_addr_t *) register_decode(so));
	} else if ((sond_type == OPERAND_TYPE_LIT) && strcmp(mnemonic, "cpvl")
			&& strcmp(mnemonic, "intr")) {
		/* Operand is a memory literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: %s", debug_operand_to_str(so, sond_type));

		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));

		if (regs.rst & REG_RST_BIT_PAGING) {
			if (!(so = paging_get_paddr(so, PAGE_PERM_RO | PAGE_PERM_RW))) {
				/* Print out the incomplete debug string */
				printf("%s ... <incomplete>)\n", dbgstr_final);

				/* Return failure */
				return 0;
			}
			
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(so)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"(0x%.8X) = 0x%.8X",
				so,
				ntohl(*(leg_addr_t *) ((char *) mm + so)));
		} else {
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(so)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				" = 0x%.8X",
				ntohl(*(leg_addr_t *) ((char *) mm + so)));
		}
	} else if (sond_type == OPERAND_TYPE_LIT) {
		/* Operand is a literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: 0x%.8X", so);
	}

	if (sond_type != OPERAND_TYPE_NONE) {
		/* Update final debug string */
		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));
	}

	/* Target Operand content */
	if (tond_type == OPERAND_TYPE_REG) {
		/* Grant that this is a register */
		if (!register_grant_any(to)) {
			printf("%s ... <incomplete>)\n", dbgstr_final);

			return 0;
		}

		/* Operand is a register */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				", TO: %s = 0x%.8X",
				debug_operand_to_str(to, tond_type),
				*(leg_addr_t *) register_decode(to));
	} else if (tond_type == OPERAND_TYPE_LIT) {
		/* Operand is a memory literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				", TO: %s",
				debug_operand_to_str(to, tond_type));

		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));

		if (regs.rst & REG_RST_BIT_PAGING) {
			if (!(to = paging_get_paddr(to, PAGE_PERM_RO | PAGE_PERM_RW))) {
				/* Print out the incomplete debug string */
				printf("%s ... <incomplete>)\n", dbgstr_final);

				/* Return failure */
				return 0;
			}
			
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(to)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"(0x%.8X) = 0x%.8X",
				to,
				ntohl(*(leg_addr_t *) ((char *) mm + to)));
		} else {
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(to)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				" = 0x%.8X",
				ntohl(*(leg_addr_t *) ((char *) mm + to)));
		}
	}

	if (tond_type != OPERAND_TYPE_NONE) {
		/* Update final debug string */
		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));
	}

	/* Print out the debug string */
	printf("%s)", dbgstr_final);
#else
	printf("%s", dbgstr_final);
#endif

	/* Return success */
	return 1;
}

int debug_instruction_leave(
		const char *mnemonic,
		leg_addr_t so,
		leg_addr_t to,
		uint8_t opcode_size,
		uint8_t sond_type,
		uint8_t tond_type) {
#if DEBUG_LEAVE_OPERAND_STATUS == 1
	char dbgstr_partial[128] = { 0 }, dbgstr_final[512] = { 0 };

	strcpy(dbgstr_final, "\t# Leave(");

	/* Source Operand content */
	if (sond_type == OPERAND_TYPE_REG) {
		/* Grant that this is a register */
		if (!register_grant_any(so)) {
			printf("%s ... <incomplete>)\n", dbgstr_final);

			return 0;
		}

		/* Operand is a register */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: %s = 0x%.8X",
				debug_operand_to_str(so, sond_type),
				*(leg_addr_t *) register_decode(so));
	} else if ((sond_type == OPERAND_TYPE_LIT) && strcmp(mnemonic, "cpvl")
			&& strcmp(mnemonic, "intr")) {
		/* Operand is a memory literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: %s", debug_operand_to_str(so, sond_type));

		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));

		if (regs.rst & REG_RST_BIT_PAGING) {
			if (!(so = paging_get_paddr(so, PAGE_PERM_RO | PAGE_PERM_RW))) {
				/* Print out the incomplete debug string */
				printf("%s ... <incomplete>)\n", dbgstr_final);

				/* Return failure */
				return 0;
			}
			
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(so)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"(0x%.8X) = 0x%.8X",
				so,
				ntohl(*(leg_addr_t *) ((char *) mm + so)));
		} else {
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(so)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				" = 0x%.8X",
				ntohl(*(leg_addr_t *) ((char *) mm + so)));
		}
	} else if (sond_type == OPERAND_TYPE_LIT) {
		/* Operand is a literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"SO: 0x%.8X", so);
	}

	if (sond_type != OPERAND_TYPE_NONE) {
		/* Update final debug string */
		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));
	}

	/* Target Operand content */
	if (tond_type == OPERAND_TYPE_REG) {
		/* Grant that this is a register */
		if (!register_grant_any(to)) {
			printf("%s ... <incomplete>)\n", dbgstr_final);

			return 0;
		}

		/* Operand is a register */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				", TO: %s = 0x%.8X",
				debug_operand_to_str(to, tond_type),
				*(leg_addr_t *) register_decode(to));
	} else if (tond_type == OPERAND_TYPE_LIT) {
		/* Operand is a memory literal */
		snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				", TO: %s", debug_operand_to_str(to, tond_type));

		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));

		if (regs.rst & REG_RST_BIT_PAGING) {
			if (!(to = paging_get_paddr(to, PAGE_PERM_RO | PAGE_PERM_RW))) {
				/* Print out the incomplete debug string */
				printf("%s ... <incomplete>)\n", dbgstr_final);

				/* Return failure */
				return 0;
			}
			
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(to)) {
				printf("%s ... <incomplete>)\n", dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				"(0x%.8X) = 0x%.8X",
				to,
				ntohl(*(leg_addr_t *) ((char *) mm + to)));
		} else {
			/* Grant normal zone and check ram boundaries */
			if (!mm_grant_zone_normal(to)) {
				printf("%s ... <incomplete>)\n",
					dbgstr_final);

				return 0;
			}

			snprintf(dbgstr_partial, sizeof(dbgstr_partial) - 1,
				" = 0x%.8X",
				ntohl(*(leg_addr_t *) ((char *) mm + to)));
		}
	}

	if (tond_type != OPERAND_TYPE_NONE) {
		/* Update final debug string */
		strncat(dbgstr_final, dbgstr_partial,
			(sizeof(dbgstr_final) - 1) - (strlen(dbgstr_final) + 1));
	}

	/* Print out the debug string */
	printf("%s)", dbgstr_final);
#endif
	printf("\n");

	/* Return success */
	return 1;
}

void debug_interrupt_caught(uint8_t intrid) {
	printf("HARDWARE INTERRUPT: 0x%.2X\n", intrid);
}

