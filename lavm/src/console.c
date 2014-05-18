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
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "config.h"
#include "display.h"
#include "keyboard.h"
#include "pqueue.h"

static struct termios tds;
static pthread_t tworker;

static void _console_keyboard_handle(void) {
	for (;;)
		keyboard_dispatch(keyboard_input());
}

static void *_console_display_handle(void *n) {
	unsigned char disp;

	for (;;) {
		disp = (unsigned char) display_receive();

		/* NOTE: Supress compiler warning on unused write() return value */
		(ssize_t [1]) { }[0] = write(STDOUT_FILENO, &disp, 1);
	}

	return NULL;
}

static int _console_config_nonbuf(void) {
	struct termios tds_new;

	if (tcgetattr(STDIN_FILENO, &tds) < 0)
		return -1;

	tds_new = tds;

	tds_new.c_lflag |= ISIG;
	tds_new.c_lflag &= ~(ICANON | ECHO);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &tds_new) < 0)
		return -2;

	return 0;
}

static void _console_cursor_on(void) {
	/* NOTE: Supress compiler warning on unused write() return value */
	(ssize_t [1]) { }[0] = write(STDOUT_FILENO, "\x1b[?25h", 6);
}

#if 0
static void _console_cursor_off(void) {
	write(STDOUT_FILENO, "\x1b[?25l", 6);
}

#endif

static void _console_clear(void) {
	/* NOTE: Supress compiler warning on unused write() return value */
	(ssize_t [1]) { }[0] = write(STDOUT_FILENO, "\x1b[2J\x1b[;1H", 9);
}

static void _console_config_restore(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &tds);
}

static void _console_destroy(void) {
	keyboard_destroy();
	display_destroy();
}

static void _console_sig_handler(int n) {
	pthread_cancel(tworker);
	_console_destroy();
	_console_config_restore();
	exit(EXIT_SUCCESS);
}

static void _console_init(void) {
	signal(SIGINT, &_console_sig_handler);
	signal(SIGQUIT, &_console_sig_handler);
	signal(SIGTERM, &_console_sig_handler);

	_console_clear();
	_console_cursor_on();

	if (_console_config_nonbuf() < 0) {
		puts("Failed to configure terminal.");
		exit(EXIT_FAILURE);
	}

	display_init();
	keyboard_init();

	pthread_create(&tworker, NULL, &_console_display_handle, NULL);

	_console_keyboard_handle();

}

int main(void) {
	_console_init();

	return 0;
}

