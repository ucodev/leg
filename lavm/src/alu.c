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

#include "archdefs.h"
#include "register.h"
#include "fault.h"
#include "alu.h"

leg_char_t alu_add_char(leg_char_t from, leg_char_t to) {
	leg_char_t res = to + from;

	alu_add_signed_eval_uf_of(leg_char_t);

	return res;
}

leg_uchar_t alu_add_uchar(leg_uchar_t from, leg_uchar_t to) {
	leg_uchar_t res = to + from;

	alu_add_unsigned_eval_uf_of(leg_uchar_t);

	return res;
}

leg_short_t alu_add_short(leg_short_t from, leg_short_t to) {
	leg_short_t res = to + from;

	alu_add_signed_eval_uf_of(leg_short_t);

	return res;
}

leg_ushort_t alu_add_ushort(leg_ushort_t from, leg_ushort_t to) {
	leg_ushort_t res = to + from;

	alu_add_unsigned_eval_uf_of(leg_ushort_t);

	return res;
}

leg_int_t alu_add_int(leg_int_t from, leg_int_t to) {
	leg_int_t res = to + from;

	alu_add_signed_eval_uf_of(leg_int_t);

	return res;
}

leg_uint_t alu_add_uint(leg_uint_t from, leg_uint_t to) {
	leg_uint_t res = to + from;

	alu_add_unsigned_eval_uf_of(leg_uint_t);

	return res;
}

leg_long_t alu_add_long(leg_long_t from, leg_long_t to) {
	leg_long_t res = to + from;

	alu_add_signed_eval_uf_of(leg_long_t);

	return res;
}

leg_ulong_t alu_add_ulong(leg_ulong_t from, leg_ulong_t to) {
	leg_ull_t res_ext = 0;
	leg_ulong_t res = 0;

	res_ext = (leg_ull_t) to + (leg_ull_t) from;
	res = res_ext & 0xffffffff;

#if ARCH_ADDR_BITS == 32
	if (regs.rarth & REG_ARTH_BIT_ERAL1) {
		regs.ral1 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL2) {
		regs.ral2 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL3) {
		regs.ral3 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL4) {
		regs.ral4 = (res_ext & 0xffffffff00000000) >> 32;
	} else {
		alu_add_unsigned_eval_uf_of(leg_ulong_t);
	}
#elif ARCH_ADDR_BITS == 64
	alu_add_unsigned_eval_uf_of(leg_ulong_t);
	/* TODO: Extended operands not yet implemented for LEG64 */
#endif

	return res;
}

leg_char_t alu_sub_char(leg_char_t from, leg_char_t to) {
	leg_char_t res = to - from;

	alu_sub_signed_eval_uf_of(leg_char_t);

	return res;
}

leg_uchar_t alu_sub_uchar(leg_uchar_t from, leg_uchar_t to) {
	leg_uchar_t res = to - from;

	alu_sub_unsigned_eval_uf_of(leg_uchar_t);

	return res;
}

leg_short_t alu_sub_short(leg_short_t from, leg_short_t to) {
	leg_short_t res = to - from;

	alu_sub_signed_eval_uf_of(leg_short_t);

	return res;
}

leg_ushort_t alu_sub_ushort(leg_ushort_t from, leg_ushort_t to) {
	leg_ushort_t res = to - from;

	alu_sub_unsigned_eval_uf_of(leg_ushort_t);

	return res;
}

leg_int_t alu_sub_int(leg_int_t from, leg_int_t to) {
	leg_int_t res = to - from;

	alu_sub_signed_eval_uf_of(leg_int_t);

	return res;
}

leg_uint_t alu_sub_uint(leg_uint_t from, leg_uint_t to) {
	leg_uint_t res = to - from;

	alu_sub_unsigned_eval_uf_of(leg_uint_t);

	return res;
}

leg_long_t alu_sub_long(leg_long_t from, leg_long_t to) {
	leg_long_t res = to - from;

	alu_sub_signed_eval_uf_of(leg_long_t);

	return res;
}

leg_ulong_t alu_sub_ulong(leg_ulong_t from, leg_ulong_t to) {
	leg_ull_t res_ext = 0;
	leg_ulong_t res = 0;

	res_ext = (leg_ull_t) to - (leg_ull_t) from;
	res = res_ext & 0xffffffff;

#if ARCH_ADDR_BITS == 32
	if (regs.rarth & REG_ARTH_BIT_ERAL1) {
		regs.ral1 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL2) {
		regs.ral2 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL3) {
		regs.ral3 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL4) {
		regs.ral4 = (res_ext & 0xffffffff00000000) >> 32;
	} else {
		alu_sub_unsigned_eval_uf_of(leg_ulong_t);
	}
#elif ARCH_ADDR_BITS == 64
	alu_sub_unsigned_eval_uf_of(leg_ulong_t);
	/* TODO: Extended operands not yet implemented for LEG64 */
#endif

	return res;
}

leg_char_t alu_mul_char(leg_char_t from, leg_char_t to) {
	leg_char_t res = to * from;

	alu_mul_signed_eval_uf_of(leg_char_t);

	return res;
}

leg_uchar_t alu_mul_uchar(leg_uchar_t from, leg_uchar_t to) {
	leg_uchar_t res = to * from;

	alu_mul_unsigned_eval_uf_of(leg_uchar_t);

	return res;
}

leg_short_t alu_mul_short(leg_short_t from, leg_short_t to) {
	leg_short_t res = to * from;

	alu_mul_signed_eval_uf_of(leg_short_t);

	return res;
}

leg_ushort_t alu_mul_ushort(leg_ushort_t from, leg_ushort_t to) {
	leg_ushort_t res = to * from;

	alu_mul_unsigned_eval_uf_of(leg_ushort_t);

	return res;
}

leg_int_t alu_mul_int(leg_int_t from, leg_int_t to) {
	leg_int_t res = to * from;

	alu_mul_signed_eval_uf_of(leg_int_t);

	return res;
}

leg_uint_t alu_mul_uint(leg_uint_t from, leg_uint_t to) {
	leg_uint_t res = to * from;

	alu_mul_unsigned_eval_uf_of(leg_uint_t);

	return res;
}

leg_long_t alu_mul_long(leg_long_t from, leg_long_t to) {
	leg_long_t res = to * from;

	alu_mul_signed_eval_uf_of(leg_long_t);

	return res;
}

leg_ulong_t alu_mul_ulong(leg_ulong_t from, leg_ulong_t to) {
	leg_ull_t res_ext = 0;
	leg_ulong_t res = 0;

	res_ext = (leg_ull_t) to * (leg_ull_t) from;
	res = res_ext & 0xffffffff;

#if ARCH_ADDR_BITS == 32
	if (regs.rarth & REG_ARTH_BIT_ERAL1) {
		regs.ral1 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL2) {
		regs.ral2 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL3) {
		regs.ral3 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL4) {
		regs.ral4 = (res_ext & 0xffffffff00000000) >> 32;
	} else {
		alu_mul_unsigned_eval_uf_of(leg_ulong_t);
	}
#elif ARCH_ADDR_BITS == 64
	alu_mul_unsigned_eval_uf_of(leg_ulong_t);
	/* TODO: Extended operands not yet implemented for LEG64 */
#endif

	return res;
}

leg_char_t alu_div_char(leg_char_t from, leg_char_t to) {
	leg_char_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_uchar_t alu_div_uchar(leg_uchar_t from, leg_uchar_t to) {
	leg_uchar_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_short_t alu_div_short(leg_short_t from, leg_short_t to) {
	leg_short_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_ushort_t alu_div_ushort(leg_ushort_t from, leg_ushort_t to) {
	leg_ushort_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_int_t alu_div_int(leg_int_t from, leg_int_t to) {
	leg_int_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_uint_t alu_div_uint(leg_uint_t from, leg_uint_t to) {
	leg_uint_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}


leg_long_t alu_div_long(leg_long_t from, leg_long_t to) {
	leg_long_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to / from;

	return res;
}

leg_ulong_t alu_div_ulong(leg_ulong_t from, leg_ulong_t to) {
	leg_ull_t res_ext = 0;
	leg_ulong_t res = 0;

	if (!from) {
		fault_alu();
		return 0;
	}

	res_ext = (leg_ull_t) to / (leg_ull_t) from;
	res = res_ext & 0xffffffff;

#if ARCH_ADDR_BITS == 32
	if (regs.rarth & REG_ARTH_BIT_ERAL1) {
		regs.ral1 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL2) {
		regs.ral2 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL3) {
		regs.ral3 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL4) {
		regs.ral4 = (res_ext & 0xffffffff00000000) >> 32;
	}
#elif ARCH_ADDR_BITS == 64
	/* TODO: Extended operands not yet implemented for LEG64 */
#endif

	return res;
}

leg_char_t alu_mod_char(leg_char_t from, leg_char_t to) {
	leg_char_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_uchar_t alu_mod_uchar(leg_uchar_t from, leg_uchar_t to) {
	leg_uchar_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_short_t alu_mod_short(leg_short_t from, leg_short_t to) {
	leg_short_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_ushort_t alu_mod_ushort(leg_ushort_t from, leg_ushort_t to) {
	leg_ushort_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_int_t alu_mod_int(leg_int_t from, leg_int_t to) {
	leg_int_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_uint_t alu_mod_uint(leg_uint_t from, leg_uint_t to) {
	leg_uint_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_long_t alu_mod_long(leg_long_t from, leg_long_t to) {
	leg_long_t res;

	if (!from) {
		fault_alu();
		return 0;
	}

	res = to % from;

	return res;
}

leg_ulong_t alu_mod_ulong(leg_ulong_t from, leg_ulong_t to) {
	leg_ull_t res_ext = 0;
	leg_ulong_t res = 0;

	if (!from) {
		fault_alu();
		return 0;
	}

	res_ext = (leg_ull_t) to % (leg_ull_t) from;
	res = res_ext & 0xffffffff;

#if ARCH_ADDR_BITS == 32
	if (regs.rarth & REG_ARTH_BIT_ERAL1) {
		regs.ral1 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL2) {
		regs.ral2 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL3) {
		regs.ral3 = (res_ext & 0xffffffff00000000) >> 32;
	} else if (regs.rarth & REG_ARTH_BIT_ERAL4) {
		regs.ral4 = (res_ext & 0xffffffff00000000) >> 32;
	}
#elif ARCH_ADDR_BITS == 64
	/* TODO: Extended operands not yet implemented for LEG64 */
#endif

	return res;
}

leg_uchar_t alu_not_uchar(leg_uchar_t from) {
	return ~from;
}

leg_ushort_t alu_not_ushort(leg_ushort_t from) {
	return ~from;
}

leg_uint_t alu_not_uint(leg_uint_t from) {
	return ~from;
}

leg_ulong_t alu_not_ulong(leg_ulong_t from) {
	return ~from;
}

leg_uchar_t alu_xor_uchar(leg_uchar_t from, leg_uchar_t to) {
	return to ^ from;
}

leg_ushort_t alu_xor_ushort(leg_ushort_t from, leg_ushort_t to) {
	return to ^ from;
}

leg_uint_t alu_xor_uint(leg_uint_t from, leg_uint_t to) {
	return to ^ from;
}

leg_ulong_t alu_xor_ulong(leg_ulong_t from, leg_ulong_t to) {
	return to ^ from;
}

leg_uchar_t alu_and_uchar(leg_uchar_t from, leg_uchar_t to) {
	return to & from;
}

leg_ushort_t alu_and_ushort(leg_ushort_t from, leg_ushort_t to) {
	return to & from;
}

leg_uint_t alu_and_uint(leg_uint_t from, leg_uint_t to) {
	return to & from;
}

leg_ulong_t alu_and_ulong(leg_ulong_t from, leg_ulong_t to) {
	return to & from;
}

leg_uchar_t alu_or_uchar(leg_uchar_t from, leg_uchar_t to) {
	return to | from;
}

leg_ushort_t alu_or_ushort(leg_ushort_t from, leg_ushort_t to) {
	return to | from;
}

leg_uint_t alu_or_uint(leg_uint_t from, leg_uint_t to) {
	return to | from;
}


leg_ulong_t alu_or_ulong(leg_ulong_t from, leg_ulong_t to) {
	return to | from;
}

leg_uchar_t alu_shl_uchar(leg_uchar_t from, leg_uchar_t to) {
	return to << from;
}

leg_ushort_t alu_shl_ushort(leg_ushort_t from, leg_ushort_t to) {
	return to << from;
}

leg_uint_t alu_shl_uint(leg_uint_t from, leg_uint_t to) {
	return to << from;
}

leg_ulong_t alu_shl_ulong(leg_ulong_t from, leg_ulong_t to) {
	return to << from;
}

leg_uchar_t alu_shr_uchar(leg_uchar_t from, leg_uchar_t to) {
	return to >> from;
}

leg_ushort_t alu_shr_ushort(leg_ushort_t from, leg_ushort_t to) {
	return to >> from;
}

leg_uint_t alu_shr_uint(leg_uint_t from, leg_uint_t to) {
	return to >> from;
}

leg_ulong_t alu_shr_ulong(leg_ulong_t from, leg_ulong_t to) {
	return to >> from;
}

leg_uchar_t alu_rol_uchar(leg_uchar_t from, leg_uchar_t to) {
	return ((to >> ((sizeof(leg_uchar_t) << 3) - from) | (to << from)) & (leg_uchar_t) ~0);
}

leg_ushort_t alu_rol_ushort(leg_ushort_t from, leg_ushort_t to) {
	return ((to >> ((sizeof(leg_ushort_t) << 3) - from) | (to << from)) & (leg_ushort_t) ~0);
}

leg_uint_t alu_rol_uint(leg_uint_t from, leg_uint_t to) {
	return ((to >> ((sizeof(leg_uint_t) << 3) - from) | (to << from)) & (leg_uint_t) ~0);
}

leg_ulong_t alu_rol_ulong(leg_ulong_t from, leg_ulong_t to) {
	return ((to >> ((sizeof(leg_ulong_t) << 3) - from) | (to << from)) & (leg_ulong_t) ~0);
}

leg_uchar_t alu_ror_uchar(leg_uchar_t from, leg_uchar_t to) {
	return ((to << ((sizeof(leg_uchar_t) >> 3) - from) | (to >> from)) & (leg_uchar_t) ~0);
}

leg_ushort_t alu_ror_ushort(leg_ushort_t from, leg_ushort_t to) {
	return ((to << ((sizeof(leg_ushort_t) >> 3) - from) | (to >> from)) & (leg_ushort_t) ~0);
}

leg_uint_t alu_ror_uint(leg_uint_t from, leg_uint_t to) {
	return ((to << ((sizeof(leg_uint_t) >> 3) - from) | (to >> from)) & (leg_uint_t) ~0);
}

leg_ulong_t alu_ror_ulong(leg_ulong_t from, leg_ulong_t to) {
	return ((to << ((sizeof(leg_ulong_t) >> 3) - from) | (to >> from)) & (leg_ulong_t) ~0);
}

