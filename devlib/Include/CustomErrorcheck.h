#ifndef CUSTOM_ERRORCHECK_H
#define CUSTOM_ERRORCHECK_H

#include <setjmp.h>

#define max_double_limit 1.7976931348623157e308
#define min_double_limit 2.2250738585072014e-308
#define max_double_int 9007199254740992

typedef enum{
	div_zero = 1,
	is_overflow,
	is_underflow,
	ex_dot,
	neg_factorial
}error_code;

int has_inf(double value, double next_val);
extern jmp_buf has_error;

#endif
