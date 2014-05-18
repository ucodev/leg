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

#include <signal.h>

#include "sighandler.h"
#include "fault.h"

static void _sig_handler(int n) {
	fault_mc();
}

void sighandler_init(void) {
	signal(SIGFPE, &_sig_handler);
	signal(SIGTERM, &_sig_handler);
	signal(SIGHUP, &_sig_handler);
	signal(SIGSEGV, &_sig_handler);
	signal(SIGILL, &_sig_handler);
	signal(SIGINT, &_sig_handler);
	signal(SIGQUIT, &_sig_handler);
}


