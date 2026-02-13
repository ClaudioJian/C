#include <windows.h>
#include <stdio.h>

//custom header files
#include "CustomErrorcheck.h"
#include "CustomMath.h"

num layer4(char **);

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
		number.value.mantissa = get_num(ptr);
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
	SetDllDirectoryA("lib");
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