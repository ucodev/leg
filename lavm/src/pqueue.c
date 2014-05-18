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
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "pqueue.h"


/* Global functions */
struct pq_ipc *pq;


/* Functions */
struct pq_ipc *pqueue_init(int key, int flags) {
	struct pq_ipc *_pq;

	if (!(_pq = malloc(sizeof(struct pq_ipc))))
		return NULL;

	memset(_pq, 0, sizeof(struct pq_ipc));

	_pq->key = (key_t) key;
	_pq->flags = flags;

	return _pq;
}

void pqueue_destroy(struct pq_ipc *_pq) {
	free(_pq);
}

int pqueue_create(struct pq_ipc *_pq) {
	return (_pq->msg_qid = msgget(_pq->key, _pq->flags));
}

int pqueue_recv(struct pq_ipc *_pq, int mtype) {
	return (_pq->data_size = msgrcv(_pq->msg_qid, &_pq->msg_buf, PQ_MSG_SZ, mtype, MSG_NOERROR));
}

int pqueue_recv_nowait(struct pq_ipc *_pq, int mtype) {
	return (_pq->data_size = msgrcv(_pq->msg_qid, &_pq->msg_buf, PQ_MSG_SZ, mtype, IPC_NOWAIT));
}

int pqueue_send(struct pq_ipc *_pq, int mtype) {
	_pq->msg_buf.type = mtype;
	return msgsnd(_pq->msg_qid, &_pq->msg_buf, _pq->data_size, 0);
}

int pqueue_send_nowait(struct pq_ipc *_pq, int mtype) {
	_pq->msg_buf.type = mtype;
	return msgsnd(_pq->msg_qid, &_pq->msg_buf, _pq->data_size, IPC_NOWAIT);
}

void pqueue_reset_buf(struct pq_ipc *_pq) {
	memset(&_pq->msg_buf, 0, sizeof(struct pq_buf));
	_pq->data_size = 0;
}

