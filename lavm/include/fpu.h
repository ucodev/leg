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

#ifndef FPU_H
#define FPU_H

#include "archdefs.h"

/* Prototypes */
leg_float_t fpu_add_float(leg_float_t from, leg_float_t to);
leg_double_t fpu_add_double(leg_float_t from, leg_float_t to);
leg_float_t fpu_sub_float(leg_float_t from, leg_float_t to);
leg_double_t fpu_sub_double(leg_float_t from, leg_float_t to);
leg_float_t fpu_mul_float(leg_float_t from, leg_float_t to);
leg_double_t fpu_mul_double(leg_float_t from, leg_float_t to);
leg_float_t fpu_div_float(leg_float_t from, leg_float_t to);
leg_double_t fpu_div_double(leg_float_t from, leg_float_t to);
leg_float_t fpu_mod_float(leg_float_t from, leg_float_t to);
leg_double_t fpu_mod_double(leg_float_t from, leg_float_t to);

#endif
