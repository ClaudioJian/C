#include <stdio.h>
#include <setjmp.h>
#include "CustomErrorcheck.h"
#include "corelogic.h"
#include "CustomMath.h"


int main(){

	while(1){
		char string[1024];
		double result;
		printf(":");
		if(fgets(string,1024,stdin)){
			char *ptr = string;
			char *str_head = ptr;
			
			num result = layer4(&ptr); //skip problem for now
			
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
				}//else printf("result:%g",result);
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