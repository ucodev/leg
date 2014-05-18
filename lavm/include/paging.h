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

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <arpa/inet.h>

#define PAGE_PERM_RO	0x01
#define PAGE_PERM_RW	0x02
#define PAGE_PERM_EXEC	0x04

#define paging_size(pgptr)	(ntohl(pgptr->size))
#define paging_perm_is_ro(pgptr)	(ntohl(pgptr->flags) & PAGE_PERM_RO)
#define paging_perm_is_rw(pgptr)	(ntohl(pgptr->flags) & PAGE_PERM_RW)
#define paging_perm_is_exec(pgptr)	(ntohl(pgptr->flags) & PAGE_PERM_EXEC)
#define paging_translate_laddr(pgptr, lgaddr) (ntohl(pgptr->paddr) + (ntohl(pgptr->laddr) - lgaddr))

#pragma pack(push)
#pragma pack(1)
struct page {
	leg_addr_t laddr;
	leg_addr_t paddr;
	leg_addr_t size;
	leg_addr_t flags;
	leg_addr_t next;
	leg_addr_t prev;
};
#pragma pack(pop)

/* Prototypes */
struct page *paging_get_laddr_page(leg_addr_t laddr);
leg_addr_t paging_get_paddr(leg_addr_t laddr, leg_addr_t flags);

#endif

