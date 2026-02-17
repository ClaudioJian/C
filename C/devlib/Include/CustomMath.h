//to use this dll, the arguments should be passed by reference, and the return value is always void(changes directly value). 
//Mostly of result and arguments are in the struct full* passed by reference.
//full* is a struct that contains a double mantissa and an int exponent [scientific notation (mantissa * 10^exponent)]


#ifndef CUSTOMMATH_H
#define CUSTOMMATH_H

//global
extern double pow10n[16];

//local

//variable struct
typedef struct{
	double mantissa;
	int exponent;
}full;

typedef struct{
	full value;
	full divisor;
}num;

//scientific notation functions
void normalize(full *val);
void longlong_cast(full *val);

//math operations
double get_num(char **ptr);
double pow_base10(int exp);
void multiply(full *val, full* next_val);
void division(full *val, full* next_val);
void add(full *val, full* next_val);

#endif