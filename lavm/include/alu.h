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

#ifndef ALU_H
#define ALU_H

#include "archdefs.h"

/* Macros */
#define alu_add_signed_eval_uf_of(type)	(regs.rarth |= (to > 0 ? to : -to) > ((((type) ~0) >> 1) - (from > 0 ? from : -from)) ? ((to < 0) && (from < 0) ? REG_ARTH_BIT_UF : REG_ARTH_BIT_OF) : 0)
#define alu_add_unsigned_eval_uf_of(type) (regs.rarth |= to > (((type) ~0) - from) ? REG_ARTH_BIT_OF : 0)
#define alu_sub_signed_eval_uf_of(type) (regs.rarth |= ((to > 0 ? to : -to) > (((type) ~0) >> 1) - (from > 0 ? from : -from)) ? ((to < 0) ? REG_ARTH_BIT_OF : (from < 0) ? REG_ARTH_BIT_UF : 0) : 0)
#define alu_sub_unsigned_eval_uf_of(type) (regs.rarth |= to < from ? REG_ARTH_BIT_UF : 0)
#define alu_mul_signed_eval_uf_of(type) (regs.rarth |= to ? (((((type) ~0) >> 1) / to) > from ? REG_ARTH_BIT_OF : (from < 0) || (to < 0) ? REG_ARTH_BIT_UF : 0) : 0)
#define alu_mul_unsigned_eval_uf_of(type) (regs.rarth |= to ? ((((type) ~0) / to) > from ? REG_ARTH_BIT_OF : 0) : 0)

/* Prototypes */
// Arithmetic
leg_char_t alu_add_char(leg_char_t, leg_char_t);
leg_uchar_t alu_add_uchar(leg_uchar_t, leg_uchar_t);
leg_short_t alu_add_short(leg_short_t, leg_short_t);
leg_ushort_t alu_add_ushort(leg_ushort_t, leg_ushort_t);
leg_int_t alu_add_int(leg_int_t, leg_int_t);
leg_uint_t alu_add_uint(leg_uint_t, leg_uint_t);
leg_long_t alu_add_long(leg_long_t from, leg_long_t to);
leg_ulong_t alu_add_ulong(leg_ulong_t from, leg_ulong_t to);
leg_float_t alu_add_float(leg_float_t from, leg_float_t to);
leg_double_t alu_add_double(leg_double_t from, leg_double_t to);
leg_char_t alu_sub_char(leg_char_t, leg_char_t);
leg_uchar_t alu_sub_uchar(leg_uchar_t, leg_uchar_t);
leg_short_t alu_sub_short(leg_short_t, leg_short_t);
leg_ushort_t alu_sub_ushort(leg_ushort_t, leg_ushort_t);
leg_int_t alu_sub_int(leg_int_t, leg_int_t);
leg_uint_t alu_sub_uint(leg_uint_t, leg_uint_t);
leg_long_t alu_sub_long(leg_long_t from, leg_long_t to);
leg_ulong_t alu_sub_ulong(leg_ulong_t from, leg_ulong_t to);
leg_float_t alu_sub_float(leg_float_t from, leg_float_t to);
leg_double_t alu_sub_double(leg_double_t from, leg_double_t to);
leg_char_t alu_mul_char(leg_char_t, leg_char_t);
leg_uchar_t alu_mul_uchar(leg_uchar_t, leg_uchar_t);
leg_short_t alu_mul_short(leg_short_t, leg_short_t);
leg_ushort_t alu_mul_ushort(leg_ushort_t, leg_ushort_t);
leg_int_t alu_mul_int(leg_int_t, leg_int_t);
leg_uint_t alu_mul_uint(leg_uint_t, leg_uint_t);
leg_long_t alu_mul_long(leg_long_t from, leg_long_t to);
leg_ulong_t alu_mul_ulong(leg_ulong_t from, leg_ulong_t to);
leg_float_t alu_mul_float(leg_float_t from, leg_float_t to);
leg_double_t alu_mul_double(leg_double_t from, leg_double_t to);
leg_char_t alu_div_char(leg_char_t, leg_char_t);
leg_uchar_t alu_div_uchar(leg_uchar_t, leg_uchar_t);
leg_short_t alu_div_short(leg_short_t, leg_short_t);
leg_ushort_t alu_div_ushort(leg_ushort_t, leg_ushort_t);
leg_int_t alu_div_int(leg_int_t, leg_int_t);
leg_uint_t alu_div_uint(leg_uint_t, leg_uint_t);
leg_long_t alu_div_long(leg_long_t from, leg_long_t to);
leg_ulong_t alu_div_ulong(leg_ulong_t from, leg_ulong_t to);
leg_float_t alu_div_float(leg_float_t from, leg_float_t to);
leg_double_t alu_div_double(leg_double_t from, leg_double_t to);
leg_char_t alu_mod_char(leg_char_t, leg_char_t);
leg_uchar_t alu_mod_uchar(leg_uchar_t, leg_uchar_t);
leg_short_t alu_mod_short(leg_short_t, leg_short_t);
leg_ushort_t alu_mod_ushort(leg_ushort_t, leg_ushort_t);
leg_int_t alu_mod_int(leg_int_t, leg_int_t);
leg_uint_t alu_mod_uint(leg_uint_t, leg_uint_t);
leg_long_t alu_mod_long(leg_long_t from, leg_long_t to);
leg_ulong_t alu_mod_ulong(leg_ulong_t from, leg_ulong_t to);
leg_float_t alu_mod_float(leg_float_t from, leg_float_t to);
leg_double_t alu_mod_double(leg_double_t from, leg_double_t to);

// Logic
leg_uchar_t alu_not_uchar(leg_uchar_t from);
leg_ushort_t alu_not_ushort(leg_ushort_t from);
leg_uint_t alu_not_uint(leg_uint_t from);
leg_ulong_t alu_not_ulong(leg_ulong_t from);
leg_uchar_t alu_xor_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_xor_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_xor_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_xor_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_and_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_and_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_and_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_and_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_or_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_or_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_or_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_or_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_shl_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_shl_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_shl_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_shl_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_shr_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_shr_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_shr_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_shr_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_rol_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_rol_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_rol_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_rol_ulong(leg_ulong_t from, leg_ulong_t to);
leg_uchar_t alu_ror_uchar(leg_uchar_t from, leg_uchar_t to);
leg_ushort_t alu_ror_ushort(leg_ushort_t from, leg_ushort_t to);
leg_uint_t alu_ror_uint(leg_uint_t from, leg_uint_t to);
leg_ulong_t alu_ror_ulong(leg_ulong_t from, leg_ulong_t to);

#endif

