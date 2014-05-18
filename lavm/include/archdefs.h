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

#ifndef ARCHDEFS_H
#define ARCHDEFS_H

#include <stdint.h>

#define ARCH_ADDR_BITS		32

#if ARCH_ADDR_BITS == 32
 typedef uint32_t leg_addr_t;
 typedef int8_t leg_char_t;
 typedef uint8_t leg_uchar_t;
 typedef int16_t leg_short_t;
 typedef uint16_t leg_ushort_t;
 typedef int32_t leg_int_t;
 typedef uint32_t leg_uint_t;
 typedef int32_t leg_long_t;
 typedef uint32_t leg_ulong_t;
 typedef int64_t leg_ll_t;
 typedef uint64_t leg_ull_t;
 typedef float leg_float_t;
 typedef double leg_double_t;
#elif ARCH_ADDR_BITS == 64
 typedef uint64_t leg_addr_t;
 typedef int8_t leg_char_t;
 typedef uint8_t leg_uchar_t;
 typedef int16_t leg_short_t;
 typedef uint16_t leg_ushort_t;
 typedef int32_t leg_int_t;
 typedef uint32_t leg_uint_t;
 typedef int64_t leg_long_t;
 typedef uint64_t leg_ulong_t;
 typedef int64_t leg_ll_t;
 typedef uint64_t leg_ull_t;
 typedef float leg_float_t;
 typedef double leg_double_t;
#else
 #error "Invalid value for ARCH_ADDR_BITS."
#endif

/* Register IDs */
#define REG_RIP			0x00	/* Instruction pointer */
#define REG_RST			0x04	/* Status register */
#define REG_RFF			0x08	/* Fault Flags */
#define REG_RFA			0x0C	/* Fault Handler Address */
#define REG_RBT			0x10	/* Base task structure address */
#define REG_RCT			0x14	/* Current task structure address */
#define REG_RPA			0x18	/* Paging Address Register */
#define REG_RRA			0x1C	/* Return Address */
#define REG_RSA			0x20	/* Stack Address */
#define REG_RCMP		0x24	/* Comparator Register */
#define REG_RLGIC		0x28	/* Logic Register */
#define REG_RARTH		0x2C	/* Arithmetic Register */
#define REG_RGP1		0x30	/* General Purpose */
#define REG_RGP2		0x34	/* General Purpose */	
#define REG_RGP3		0x38	/* General Purpose */ 
#define REG_RGP4		0x3C	/* General Purpose */ 
#define REG_RGP5		0x40	/* General Purpose */ 
#define REG_RGP6		0x44	/* General Purpose */ 
#define REG_RGP7		0x48	/* General Purpose */ 
#define REG_RGP8		0x4C	/* General Purpose */ 
#define REG_RAL1		0x50	/* Arithmetic */ 
#define REG_RAL2		0x54	/* Arithmetic */ 
#define REG_RAL3		0x58	/* Arithmetic */
#define REG_RAL4		0x5C	/* Arithmetic */
#define REG_RFP1		0x60	/* Floating Point */
#define REG_RFP2		0x64	/* Floating Point */
#define REG_RFP3		0x68	/* Floating Point */
#define REG_RFP4		0x6C	/* Floating Point */

/* Number of Architecture Registers */
#define REG_NUM			28

/* Status Register (RST)
 *
 * +-----+------------------------------+-------------------------------+
 * | bit | Unset			| Set				|
 * +-----+------------------------------+-------------------------------+
 * |  0	 | Interrupts Enabled		| Interrupts Enabled		|
 * |  1	 | Fault Handling Enabled	| Fault Handling Enabled	|
 * |  2	 | Task Registers Disabled	| Task Registers Enabled	|
 * |  3	 | Privilege Level 0		| Privilege Level 1		|
 * |  4	 | Paging Disabled		| Paging Enabled		|
 * +-----+------------------------------+-------------------------------+
 *
 */
#define REG_RST_BIT_INTR	0x01	/* Set to enable interrupts */
#define REG_RST_BIT_FH		0x02	/* Set to enable fault handling */
#define REG_RST_BIT_TSK		0x04	/* Set to enable task registers */
#define REG_RST_BIT_LOWPRIV	0x08	/* Set to drop privilege level to 1 */
#define REG_RST_BIT_PAGING	0x10	/* Set to enable paging */

/* Comparator register */
#define REG_RCMP_BIT_RESULT	0x01
#define REG_RCMP_BIT_NE		0x02
#define REG_RCMP_BIT_GT		0x04
#define REG_RCMP_BIT_LT		0x08
#define REG_RCMP_BIT_EQ		0x10
#define REG_RCMP_BITS_GE	0x14	/* REG_RCMP_BIT_EQ | REG_RCMP_BIT_GT */
#define REG_RCMP_BITS_LE	0x18	/* REG_RCMP_BIT_EQ | REG_RCMP_BIT_LT */

/* Logic register */
#define REG_RLGIC_BIT_NOT	0x01
#define REG_RLGIC_BIT_XOR	0x02
#define REG_RLGIC_BIT_AND	0x04
#define REG_RLGIC_BIT_OR	0x08
#define REG_RLGIC_BIT_SHL	0x10
#define REG_RLGIC_BIT_SHR	0x20
#define REG_RLGIC_BIT_ROL	0x40
#define REG_RLGIC_BIT_ROR	0x80
#define REG_RLGIC_BIT_8BITOP	0x10000
#define REG_RLGIC_BIT_16BITOP	0x20000
#define REG_RLGIC_BIT_32BITOP	0x40000	/* LEG64 only */


/* Arithmetic register */
#define REG_ARTH_BIT_MUL	0x01
#define REG_ARTH_BIT_DIV	0x02
#define REG_ARTH_BIT_SUB	0x04
#define REG_ARTH_BIT_ADD	0x08
#define REG_ARTH_BIT_MOD	0x10
#define REG_ARTH_BIT_SIGNED	0x20	/* Signed arithmetic operation */
#define REG_ARTH_BIT_OF		0x40	/* Set when overflow occur */
#define REG_ARTH_BIT_UF		0x80	/* Set when underflow occur */
#define REG_ARTH_BIT_ERAL1	0x100	/* Extended operand as RAL1 */
#define REG_ARTH_BIT_ERAL2	0x200	/* Extended operand as RAL2 */
#define REG_ARTH_BIT_ERAL3	0x400	/* Extended operand as RAL3 */
#define REG_ARTH_BIT_ERAL4	0x800	/* Extended operand as RAL4 */
#define REG_ARTH_BIT_ERFP1	0x1000	/* Extended operand as RFP1 */
#define REG_ARTH_BIT_ERFP2	0x2000	/* Extended operand as RFP2 */
#define REG_ARTH_BIT_ERFP3	0x4000	/* Extended operand as RFP3 */
#define REG_ARTH_BIT_ERFP4	0x8000	/* Extended operand as RFP4 */
#define REG_ARTH_BIT_8BITOP	0x10000
#define REG_ARTH_BIT_16BITOP	0x20000
#define REG_ARTH_BIT_32BITOP	0x40000	/* LEG64 only */

/* Fault Flags */
#define FAULT_MC		0x01
#define FAULT_BAD_MM		0x02
#define FAULT_BAD_REG		0x04
#define FAULT_BAD_REGVAL	0x08
#define FAULT_BAD_OPERVAL	0x10
#define FAULT_ILL_INTR		0x20
#define FAULT_ILL_OPCODE	0x40
#define FAULT_FPU		0x80
#define FAULT_ALU		0x100
#define FAULT_PAGE_PERM		0x200
#define FAULT_NO_PAGE		0x400
#define FAULT_NO_PRIV		0x800
#define FAULT_IO_OP		0x1000
#define FAULT_INTR		0x2000

/* Memory structure */
#if ARCH_ADDR_BITS == 32
 /* Interrupt Vector memory region for LEG32: 0x00 - 0x3F8 */
 #define MM_SIZE_MAX		0xFFFFFFFFUL
 #define MM_ZONE_IV		0x3F8
 #define MM_ZONE_NORMAL  	0x3F8
#elif ARCH_ADDR_BITS == 64
 /* Interrupt Vector memory region for LEG64: 0x00 - 0x5F4 */
 #define MM_SIZE_MAX		0xFFFFFFFFFFFFFFFFULL
 #define MM_ZONE_IV		0x5F4
 #define MM_ZONE_NORMAL  	0x5F4
#else
 #error "Invalid value for ARCH_ADDR_BITS"
#endif

/* Memory management */
#define MM_PAGING_SUPPORT	1

/* Interrupts */
#define INTR_01			0x01	/* Interrupt vector customization
					 * rgp1 - Interrupt ID
					 * rgp2 - Flags
					 * rgp3 - Handler address
					 */
#define INTR_03			0x03	/* Halt system */
#define INTR_09			0x09	/* Keyboard Input Interrupt
					 * rgp1: keyb input code
					 */
#define INTR_0A			0x0A	/* Display Output Interrupt
					 * rgp1 & 0xFF: ASCII output
					 */
#define INTR_0B			0x0B	/* Storage I/O Interrupt
					 * rgp1 & 0xFFFF: Storage ID
					 * rgp1 & 0xFFFF0000: Flags
					 * 	Flags:
					 *	  0x01 - Read
					 *	  0x02 - Write
					 *	  0x04 - Extended Offset
					 * rgp2: Storage Data Offset
					 * rgp3: Storage Data Size
					 * rgp4: Data Buffer Memory Address
					 * rgp5: Extended Data Offset (H32-bit)
					 */
#define INTR_0D			0x0D	/* Page cache invalidation
					 * rgp1: Base Physical Address
					 */
#define INTR_0F			0x0F	/* Timer configuration interrupt
					 * rgp1: Timer ID
					 * rgp2: Granularity Flag
					 * rgp3: Time to Expire
					 */
#define INTR_10			0x10	/* Timer expired interrupt */

/* Instruction set */
#define INSTRUCTION_SET_SIZE	14

/* Interrupt vector */
#define INTERRUPT_VECTOR_SIZE		0x7F
#define INTERRUPT_VECTOR_NR_RESERVED	0x1F

/* Hardware specifications */
#define HW_STOR_MAX		32	/* Maximum number of storage drives */
#define HW_RAM_MAX		MM_SIZE_MAX /* Maximum system memory */

/* Page structure size */
#if ARCH_ADDR_BITS == 32
 #define PAGE_STRUCT_SIZE	24
#elif ARCH_ADDR_BITS == 64
 #define PAGE_STRUCT_SIZE	48
#else
 #error "Invalid value for ARCH_ADDR_BITS"
#endif

/* Timers */
#define TIMER_NUM_MAX		8

/* Task structure specifications */
#define TASK_STRUCT_SIZE	((REG_NUM * (ARCH_ADDR_BITS / 8)) + 4)

/* Operand Types */
#define OPERAND_TYPE_NONE	0
#define OPERAND_TYPE_REG	1	/* Operand type is register */
#define OPERAND_TYPE_LIT	2	/* Operand type is literal */

#endif

