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

#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdint.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "config.h"

/* structs */
struct pq_buf {
	long		type;
	char		data[PQ_MSG_SZ];
};

struct pq_ipc {
	int		msg_qid;
	key_t		key;
	int		flags;
	struct pq_buf	msg_buf;
	uint32_t	data_size;

};

/* Prototypes */
struct pq_ipc *pqueue_init(int, int);
void pqueue_destroy(struct pq_ipc *);
int pqueue_create(struct pq_ipc *);
int pqueue_recv(struct pq_ipc *, int);
int pqueue_recv_nowait(struct pq_ipc *, int);
int pqueue_send(struct pq_ipc *, int);
int pqueue_send_nowait(struct pq_ipc *, int);
void pqueue_reset_buf(struct pq_ipc *);

/* External variables */
extern struct pq_ipc *pq;

#endif
