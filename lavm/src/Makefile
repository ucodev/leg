CC=`cat ../.compiler`
CCFLAGS=-D_REENTRANT -D_POSIX_C_SOURCE -D_XOPEN_SOURCE -D_LARGEFILE64_SOURCE -I../include -g -O2 -Wall -Werror -std=c1x -c
CCFLAGS_NOOPTIMIZE=-D_REENTRANT -D_POSIX_C_SOURCE -D_XOPEN_SOURCE -g -O0 -fno-builtin -Wall -Werror -std=c1x -c
CCFLAGS_GNUSRC=-D_REENTRANT -D_GNU_SOURCE -I../include -g -O2 -Wall -Werror -std=c1x -c
TARGET_VM_BIN=lavm
TARGET_BINST_BIN=binst
TARGET_CONSOLE_BIN=console

compile:
	${CC} ${CCFLAGS} config.c
	${CC} ${CCFLAGS} binst.c
	${CC} ${CCFLAGS} register.c
	${CC} ${CCFLAGS} instruction.c
	${CC} ${CCFLAGS} interrupt.c
	${CC} ${CCFLAGS} mm.c
	${CC} ${CCFLAGS} fault.c
	${CC} ${CCFLAGS} init.c
	${CC} ${CCFLAGS} run.c
	${CC} ${CCFLAGS} io.c
	${CC} ${CCFLAGS} vm.c
	${CC} ${CCFLAGS} paging.c
	${CC} ${CCFLAGS} task.c
	${CC} ${CCFLAGS} privilege.c
	${CC} ${CCFLAGS_GNUSRC} timer.c
	${CC} ${CCFLAGS} sighandler.c
	${CC} ${CCFLAGS} debug.c
	${CC} ${CCFLAGS} pqueue.c
	${CC} ${CCFLAGS} display.c
	${CC} ${CCFLAGS} keyboard.c
	${CC} ${CCFLAGS} console.c
	${CC} ${CCFLAGS} alu.c
	${CC} ${CCFLAGS} fpu.c
	${CC} -pthread -o ${TARGET_VM_BIN} config.o register.o instruction.o interrupt.o mm.o fault.o init.o run.o io.o vm.o paging.o task.o privilege.o timer.o sighandler.o debug.o pqueue.o alu.o fpu.o
	${CC} -o ${TARGET_BINST_BIN} binst.o
	${CC} -pthread -o ${TARGET_CONSOLE_BIN} console.o keyboard.o display.o pqueue.o

clean:
	rm -f *.o
	rm -f ${TARGET_VM_BIN}
	rm -f ${TARGET_BINST_BIN}
	rm -f ${TARGET_CONSOLE_BIN}

install:
	cp lavm /usr/local/bin/lavm_binst
	cp console /usr/local/bin/lavm_console
	cp lavm /usr/local/bin/

