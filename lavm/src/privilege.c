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

int privilege_reg_req_write(int level, leg_addr_t reg) {
	/* CPU Privilege Level 0 have direct write access to any register
	 * except RIP
	 */
	if (!level && (reg != REG_RIP))
		return 1;

	/* CPU Privilege Level 1 may only modify registers >= REG_RSA */
	if (level && (reg < REG_RSA)) {
		fault_no_priv();
		return 0;
	}

	return 1;
}

int privilege_reg_req_read(int level, leg_addr_t reg) {
	/* All Privilege Levels have direct read access to every register */
	return 1;
}

int privilege_get_current(void) {
	return !!(regs.rst & REG_RST_BIT_LOWPRIV);
}

