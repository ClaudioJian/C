#include <setjmp.h>
#include "CustomErrorcheck.h"

jmp_buf has_error;

int has_inf(double value, double next_val){
	if(next_val==0) return 0;
	
	if (value < 1.0 && next_val < 1.0) {
		if(value < min_double_limit/next_val) longjmp(has_error,is_underflow);
	}
	else if(value > max_double_limit/next_val) longjmp(has_error,is_overflow);
	
	return 0;
}