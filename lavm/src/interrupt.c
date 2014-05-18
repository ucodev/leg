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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "archdefs.h"
#include "register.h"
#include "fault.h"
#include "io.h"
#include "task.h"
#include "interrupt.h"
#include "privilege.h"
#include "mm.h"
#include "paging.h"
#include "timer.h"
#include "vm.h"
#include "debug.h"
#include "pqueue.h"

/* Interrupt vector
 *
 * 0x01 ... INTERRUPT_VECTOR_NR_RESERVED - Reserved, unmodifiable interrupts
 * INTERRUPT_VECTOR_NR_RESERVED ... INTERRUPT_VECTOR_SIZE - Costumizable
 * 							    Interrupts
 */
volatile struct intrv_entry intrv[INTERRUPT_VECTOR_SIZE] = { [ 0 ... INTERRUPT_VECTOR_SIZE - 1] = { 0, 0 } };

void interrupt_hw_check(void) {
	if (!(regs.rst & REG_RST_BIT_INTR))
		return;

	/* Check for keyboard interrupt */
	while (pqueue_recv_nowait(pq, INTR_09) > 0) {
#ifdef DEBUG
		debug_interrupt_caught(INTR_09);
#endif
		interrupt_int09(*(uint32_t *) memcpy((uint32_t [1]) { }, pq->msg_buf.data, 4));
	}

	/* Check for timer expiration */
	while (pqueue_recv_nowait(pq, INTR_10) > 0) {
#ifdef DEBUG
		debug_interrupt_caught(INTR_10);
#endif
		interrupt_int10((uint8_t) pq->msg_buf.data[0]);
	}

}

void interrupt_intvr_handler(uint8_t intr) {
	/* Check interrupt vector bondaries */
	if (intr > INTERRUPT_VECTOR_SIZE) {
		regs.rff |= FAULT_INTR;
		regs.rff |= intr << 24;
		fault_illegal_interrupt(intr);
		return;
	}

	/* Check if the interrupt handler is configured */
	if (!intrv[intr - 1].handler_addr) {
		regs.rff |= FAULT_INTR;
		regs.rff |= intr << 24;
		fault_illegal_interrupt(intr);
		return;
	}

	/* Check privilege level */
	if (privilege_get_current() > !!(intrv[intr - 1].flags & 1)) {
		regs.rff |= FAULT_INTR;
		regs.rff |= intr << 24;
		fault_no_priv();
		return;
	}

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);

	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Set RIP to interrupt handler address */
	regs.rip = intrv[intr - 1].handler_addr;
}

void interrupt_int01(leg_addr_t rgp1, leg_addr_t rgp2, leg_addr_t rgp3) {
	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x01 << 24;
		fault_no_priv();
		return;
	}

	if ((rgp1 >= (INTERRUPT_VECTOR_SIZE - 1)) ||
			(rgp1 <= INTERRUPT_VECTOR_NR_RESERVED)) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x01 << 24;
		fault_bad_oper_val(rgp1);
		return;
	}

	intrv[rgp1 - 1].flags = rgp2;
	intrv[rgp1 - 1].handler_addr = rgp3;

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);

	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Non-Trappable */
}

void interrupt_int03(void) {
	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x03 << 24;
		fault_no_priv();
		return;
	}

#ifdef DEBUG
	puts("\nSystem Halted.\n");
#endif

	/* Non-Trappable */
	vm_destroy();
}

void interrupt_int09(leg_addr_t rgp1) {
	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Properly set RGP1 with received keyboard code */
	regs.rgp1 = rgp1;

	/* Trappable */
	if (intrv[0x09 - 1].handler_addr)
		regs.rip = intrv[0x09 - 1].handler_addr;
}

void interrupt_int0a(leg_addr_t rgp1) {
	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0A << 24;
		fault_no_priv();
		return;
	}


	if (io_display_write((uint8_t) rgp1) < 0) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0A << 24;
		fault_io_op();
		return;
	}

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);

	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Non-Trappable */
}

void interrupt_int0b(
		leg_addr_t rgp1,
		leg_addr_t rgp2,
		leg_addr_t rgp3,
		leg_addr_t addr,
		leg_addr_t rgp5) {
	uint64_t offset;

	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0B << 24;
		fault_no_priv();
		return;
	}

	regs.rff |= FAULT_INTR;

	if (regs.rst & REG_RST_BIT_PAGING) {
		if (!(addr = paging_get_paddr(addr, rgp1 & 0x10000 ? PAGE_PERM_RW : (PAGE_PERM_RO | PAGE_PERM_RW))))
			return;
	}

	if (!mm_grant_zone_normal(addr))
		return;

	regs.rff &= ~FAULT_INTR;

	if ((rgp1 & 0xFFFF) > sizeof(io.fdstor)) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0B << 24;

		fault_bad_oper_val(rgp1);

		return;
	}

	if (rgp1 & 0x10000) {
		if (rgp1 & 0x40000) {
			offset = (((uint64_t) rgp5) << 32) | rgp2;
			if (io_storage_read_extended(rgp1 & 0xFFFF, addr, offset, rgp3) < 0) {
				regs.rff |= FAULT_INTR;
				regs.rff |= 0x0B << 24;
				fault_io_op();
				return;
			}
		} else {
			if (io_storage_read(rgp1 & 0xFFFF, addr, rgp2, rgp3) < 0) {
				regs.rff |= FAULT_INTR;
				regs.rff |= 0x0B << 24;
				fault_io_op();
				return;
			}
		}
	} else if (regs.rgp1 & 0x20000) {
		if (rgp1 & 0x40000) {
			offset = (((uint64_t) rgp5) << 32) | rgp2;
			if (io_storage_write_extended(rgp1 & 0xFFFF, addr, offset, rgp3) < 0) {
				regs.rff |= FAULT_INTR;
				regs.rff |= 0x0B << 24;
				fault_io_op();
				return;
			}
		} else {
			if (io_storage_write(rgp1 & 0xFFFF, addr, rgp2, rgp3) < 0) {
				regs.rff |= FAULT_INTR;
				regs.rff |= 0x0B << 24;
				fault_io_op();
				return;
			}
		}
	} else {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0B << 24;
		fault_bad_oper_val(rgp1);
		return;
	}

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);

	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Non-Trappable */
}

void interrupt_int0d(leg_addr_t rgp1) {
	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0D << 24;
		fault_no_priv();
		return;
	}

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);
	
	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* TODO: Implement page caching */

	/* Non-Trappable */
}

void interrupt_int0f(leg_addr_t rgp1, leg_addr_t rgp2, leg_addr_t rgp3) {
	struct timer *timer;

	/* Privilege Level Check */
	if (privilege_get_current()) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0F << 24;
		fault_no_priv();
		return;
	}

	if (rgp1 >= TIMER_NUM_MAX) {
		regs.rff |= FAULT_INTR;
		regs.rff |= 0x0F << 24;
		fault_bad_oper_val(rgp1);
		return;
	}

	if (!(timer = malloc(sizeof(struct timer))))
		fault_mc(); // Machine Check Fault on OOM (host)

	timer->id = rgp1;
	timer->granularity = rgp2;
	timer->tte = rgp3;

	if (pthread_create(&timer_list[rgp1], NULL, timer_setup, &timer))
		fault_mc();

	/* Update RIP before context switch */
	regs.rip += (ARCH_ADDR_BITS >> 3);

	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Non-Trappable */
}

void interrupt_int10(uint8_t rgp1) {
	/* Context switch */
	if (regs.rst & REG_RST_BIT_TSK) {
		task_save_rct();
		task_load_rbt();
	}

	/* Properly set RGP1 with received timer ID */
	regs.rgp1 = rgp1;

	/* Trappable */
	if (intrv[0x10 - 1].handler_addr)
		regs.rip = intrv[0x10 - 1].handler_addr;
}

