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
#include "paging.h"
#include "mm.h"
#include "register.h"
#include "fault.h"

struct page *paging_get_laddr_page(leg_addr_t laddr) {
	struct page *pg;

	/* Get first page from RPA  */
	pg = (struct page *) (((char *) mm) + htonl(regs.rpa));

	do {
		if ((laddr >= ntohl(pg->laddr)) && (laddr <= (ntohl(pg->laddr) + ntohl(pg->size))))
			return pg;

		pg = (struct page *) (((char *) mm) + ntohl(pg->next));
	} while (pg);

	return NULL;
}

leg_addr_t paging_get_paddr(leg_addr_t laddr, leg_addr_t flags) {
	struct page *pg;

	if (!(pg = paging_get_laddr_page(laddr))) {
	        /* If the page isn't found, generate a
		 * page fault
		 */
		fault_no_page(laddr);

		return 0;
	}

	/* Check page flags */
	if (!(ntohl(pg->flags) & flags)) {
		fault_page_perm(laddr, flags);
		return 0;
	}

	/* Set physical RIP */
	return paging_translate_laddr(pg, laddr);
}

