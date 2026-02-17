#include <stdio.h>
#include "CustomErrorcheck.h"
#include "CustomMath.h"

double pow10n[16]={1.0,1e1,1e2,1e3,1e4,1e5,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16};

//scientific notation functions
void normalize(full *val){
	//handle negative number
	int sig= 1;
	if (val->mantissa<0) {sig=-1;val->mantissa *= -1;}
		
	if(val->mantissa >= 1){
		while(val->mantissa >=10){
			val->mantissa/=10;
			val->exponent++;
		}
	}
	//handle small number
	else{
		while(val->mantissa < 1 && val->mantissa != 0){
			val->mantissa*=10;
			val->exponent--;
	}
	val->mantissa *= sig;
	return;
	}
}

void longlong_cast(full *val){
	//long long 0.n = 0
	if(val->exponent <0){
		val->mantissa = 0.0;
		val->exponent = 0;
	}else if(val->exponent<18){
		long long casted_val = val->mantissa * pow_base10(val->exponent);
		val->mantissa = casted_val;
		val->exponent = 0;
		normalize(val);
	}
	return;
}



//math operations

//string to number
double get_num(char **ptr){
    double number = 0;
    unsigned int div_i = 0;
    unsigned char has_dot = 0;
	double sig= 1;
	int has_not = 0;
    
    while(**ptr==' '){(*ptr)++;}
    
	//check sign and operator '!'(handle situation like --5 or !!5)
	while(**ptr=='-'||**ptr=='+'||**ptr=='!') {
		if(**ptr=='-') sig*= -1;
		if(**ptr=='!') has_not = (!has_not);
		(*ptr)++;
	}
	
	//number part
	while((**ptr>='0' && **ptr<='9')||(**ptr=='.')){
		if(**ptr!='.'){
			if(has_dot) div_i++;
			//10^308 is already overflow for double
			if(div_i>308 && number!= 0) longjmp(has_error,is_overflow);
			has_inf(number,10);

			number = number * 10 +(**ptr-'0');
		}
		else{
			if(has_dot) longjmp(has_error,ex_dot); //detect extra dot
			else has_dot = 1;
		}
		(*ptr)++;
	}
		
	// precision check
	if(number>= max_double_int) printf("warning: loss precision\n");
	
	if(has_dot && number!= 0) number /= pow_base10(div_i);
	if(has_not) number = (!number);
		
    while(**ptr==' '){(*ptr)++;}
	
    return number*sig;
}

double pow_base10(int exp){
	if (exp > 308) longjmp(has_error, is_overflow);
    if (exp < -324) longjmp(has_error, is_underflow); 

	double result = 1.0;
	double base = 10.0;
	int has_neg= 0;
	if(exp<0) {
		exp = -exp;
		has_neg = 1;
	}
	
	while(exp>0){
		if(exp%2==1) {
			has_inf(result,10);
			result *= base;
		}
		exp/=2;
		if(exp>0)base *= base;
	}
	if(has_neg) result = 1.0/result;
	return result;
}

//modify only val directly in memory
void multiply(full *val, full* next_val){
	(val->mantissa)*=(next_val->mantissa);
	(val->exponent)+=(next_val->exponent);
	normalize(val);
	return;
}
//modify only val directly in memory
void division(full *val, full* next_val){
	(val->mantissa)/=(next_val->mantissa);
	(val->exponent)-=(next_val->exponent);
	normalize(val);
	return;
}
//modify only val directly in memory
void add(full *val, full* next_val){
	if(val->mantissa==0.0){ 
		val->mantissa = next_val->mantissa;
		val->exponent = next_val->exponent;
		return;
	}
	if(next_val->mantissa==0.0) return; //no change for val
	
	int exp_diff = val->exponent - next_val->exponent;
	//double can only handle 15~17 decimal digits precisely
	if(exp_diff == 0) val->mantissa += next_val->mantissa;
	else {
		if(exp_diff>0 && exp_diff<16){ //val>next_val
			val->mantissa += next_val->mantissa/pow10n[exp_diff];
		}
		else if(exp_diff<0){ //next_val>val
			if(exp_diff>-16){
				val->mantissa /= pow10n[-exp_diff]; //align with next_val
				val->mantissa += next_val ->mantissa;
				val->exponent = next_val->exponent;
			}
			else{
				//when exp_diff is smaller than -16, the contribution of val is negligible, so just assign next_val to val
				val->mantissa = next_val->mantissa;
				val->exponent = next_val->exponent;
			}
		}
	}
	normalize(val);
	return;
}