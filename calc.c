#include <stdio.h>
#include <setjmp.h>

#define max_double_limit 1.7976931348623157e308
#define min_double_limit 2.2250738585072014e-308
#define max_double_int 9007199254740992

double pow10n[16]={1.0,1e1,1e2,1e3,1e4,1e5,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16};

typedef enum{
	div_zero = 1,
	is_overflow,
	is_underflow,
	ex_dot,
	neg_factorial
}error_code;

jmp_buf has_error;
typedef struct{
	double mantissa;
	int exponent;
}full;

typedef struct{
	full value;
	full divisor;
}num;

num layer2(char **ptr);
//num layer1(char **ptr); skip for now

//check overflow and underflow for multiplication
int has_inf(double value, double next_val){
	if(next_val==0) return 0;
	
	if (value < 1.0 && next_val < 1.0) {
		if(value < min_double_limit/next_val) longjmp(has_error,is_underflow);
	}
	else if(value > max_double_limit/next_val) longjmp(has_error,is_overflow);
	
	return 0;
}

//string to number
double get_number(char **ptr){
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
	while(**ptr>='0' && **ptr<='9'||**ptr=='.'){
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

//normalize number into scientific notation(1.mantissa * 10^exponent)
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

//handle parentheses
num layer0(char **ptr){
	num number;
	if(**ptr=='('){
		(*ptr)++;
		number = layer4(ptr);
		if(**ptr==')')(*ptr)++;
	}else{
		//set dividend(scientific notation)/1
		number.divisor.mantissa = 1;
		number.divisor.exponent = 0;
		number.value.mantissa = get_number(ptr);
		number.value.exponent = 0;
		normalize(&number.value);
	}
	return number;
}


// double layer1(char **ptr){  //factorial
// 	unsigned int i=0;
//     double value = get_number(ptr);
	
// 	while(**ptr=='!'||**ptr==' '){
// 		if(**ptr=='!') i++;
// 		(*ptr)++;
// 	}
// 	if(i>=1){
// 		if(value==0) return 1;
// 		if(value<0) longjmp(has_error,neg_factorial);
// 		if(value>=170&& i==1) longjmp(has_error , is_overflow);
			
// 		for(double multiplier = value - i;multiplier > 0;multiplier-=i){
// 		value *= multiplier;
// 		}
// 	}
// 	return value;
// }

//modify only val directly in memory
void multiply(full *val, full* next_val){
	(val->mantissa)*=(next_val->mantissa);
	(val->exponent)+=(next_val->exponent);
	normalize(val);
	return;
}

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

num layer2(char **ptr){ 
	num number = layer0(ptr);
	
    while(**ptr=='*'||**ptr=='/'||**ptr=='%'||**ptr=='(') {
		char op=**ptr;
		if(op!='(') (*ptr)++;
		num next_val = layer0(ptr);
		
		if(op=='%'||op=='/'){
			if(next_val.value.mantissa==0.0) longjmp(has_error,div_zero);
			else if(op=='%'){
				//(a/b)%(x/y) = ay-(bx(ay/bx))/by
				multiply(&number.value,&next_val.divisor); // y(next_number divisor) don't change| number changed into ay
				
				full ay = number.value; //ay
				full b = number.divisor; //b
				
				multiply(&b,&next_val.value); // b = bx
				
				division(&ay,&b); //ay/bx stored in a    bx not changed
				
				longlong_cast(&ay); // ay = (long long)(ay/bx)
				
				multiply(&b,&ay); //b store bx(ay/bx)
				b.mantissa = -b.mantissa;
				
				add(&number.value,&b); //ay-(bx(ay/bx))
				multiply(&number.divisor,&next_val.divisor);
			}
			else if(op=='/'){
				multiply(&number.value, &next_val.divisor);
				multiply(&number.divisor, &next_val.value);	
			}
		}
		else{
			multiply(&number.value, &next_val.value);
			multiply(&number.divisor, &next_val.divisor);
		}
		if(number.value.mantissa == number.divisor.mantissa){
			number.divisor.mantissa = number.value.mantissa = 1.0;
    }
	
	number.value.exponent -= number.divisor.exponent;
	
	return number;
	}
}

num layer3(char **ptr){
    num number = layer2(ptr);
	
    while(**ptr=='+'||**ptr=='-') {
        char op=*(*ptr)++;
        
		num next_val = layer2(ptr);
		
		if(next_val.value.mantissa == 0) continue;
		if(number.value.mantissa == 0){
			if(op =='-'){
			//negate next_val for function add
			next_val.value.mantissa *= -1;
			next_val.divisor.mantissa *= -1;
			}
			number.value = next_val.value;
			number.divisor = next_val.divisor;
		}

		//skip this line when divisors are same
		if((number.divisor.mantissa != next_val.divisor.mantissa)||(number.divisor.exponent != next_val.divisor.exponent)){
			//cross multiply
			multiply(&number.value,&next_val.divisor);
			multiply(&next_val.value,&number.divisor);
			multiply(&number.divisor,&next_val.divisor);
		}
		
		add(&number.value,&next_val.value);
		
		//easy simplification: trigger if dividend and divisor are same
		if(number.value.mantissa == number.divisor.mantissa){
			number.divisor.mantissa = number.value.mantissa = 1.0;
		}
		number.value.exponent -= number.divisor.exponent;
	}
	return number;
}

num layer4(char **ptr){
    num number = layer3(ptr);
	
    while(**ptr=='>'||**ptr=='<'||**ptr=='='||**ptr=='!') {
        char op=(*(*ptr)++);
		char op_adjacent = **ptr;
		if(**ptr == '=')  (*ptr)++;
		
		num next_val = layer3(ptr);
		//   [x/y > a/b] -> [xb/1 > ya/1] -> [xb > ya]
		multiply(&number.value,&next_val.divisor);
		multiply(&next_val.value,&number.divisor);
		number.divisor.mantissa = 1.0;
		number.divisor.exponent = 0.0;
		
		int is_equal = (number.value.mantissa == next_val.value.mantissa)&&(number.value.exponent == next_val.value.exponent);
        if(op_adjacent == '='){
			if(op=='!') {
				number.value.mantissa = !is_equal;
				number.value.exponent = 0.0;
				continue;}
			if(is_equal){
				number.value.mantissa = is_equal;
				number.value.exponent = 0.0;
				continue;
			}
		}
		
		//compare magnitudes when exponents are different; compare mantissas when exponents are same or signal between numbers <0.
		int signal = number.value.mantissa * next_val.value.mantissa;
		int boolean;
		if (signal < 0 || number.value.exponent == next_val.value.exponent) {
			boolean = (number.value.mantissa > next_val.value.mantissa);
		} else {
			boolean = (number.value.exponent > next_val.value.exponent);
			if (number.value.mantissa < 0) boolean = !boolean; // Flip for negative magnitudes
		}
		
		if(op == '<')  boolean = !boolean;
		number.value.mantissa = (double)boolean;
		number.value.exponent = 0.0;
		
		//easy simplification: trigger if dividend and divisor are same
		if(number.value.mantissa == number.divisor.mantissa){
			number.divisor.mantissa = number.value.mantissa = 1.0;
		}
	}
	
    return number;
}

int main()
{
	while(1){
		char string[1024];
		double result;
		printf(":");
		if(fgets(string,1024,stdin)){
			char *ptr = string;
			char *str_head = ptr;
			
			//result = layer4(&ptr); //skip problem for now
			
			//error check
			int error_val = setjmp(has_error);
			if(error_val == 0){
				if(*ptr!='\n'&& *ptr != '\0'){
					printf("error!expected an operator after number!\n");
					printf("here -> ...");
					char *ptr_temp = ptr;
					for(unsigned char i = 0;i<=3;i++){
						ptr_temp--;
						if(ptr_temp == str_head)break;
					}
					for(unsigned char i = 0;i<=6;i++){
						printf("%c",*ptr_temp);
						if(*(++ptr_temp)=='\n')break;
					}
				}else printf("result:%g",result);
			} 
			else{
				printf("error code %i:",error_val);
				switch(error_val){
					case div_zero:			printf("division by 0");break;
					case is_overflow:    	printf("overflow imminent\n"); break;
					case is_underflow: 		printf("underflow\n"); break;
					case ex_dot: 			printf("detected extra dot\n"); break;
					case neg_factorial: 	printf("Factorial of negative number is undefinied\n"); break;
				}
			}
		}
		else printf("error in fgets...\n");
		
		char answer[40];
		printf("\ndo you want to restart(enter \"q\" to quit program)?");
		scanf("%39s",&answer);
		int i =0;
		while (answer[i] == ' ' || answer[i] == '\t') i++;
		if(answer[i]=='q'||answer[i]=='Q') break;
	}
	
    return 0;
}