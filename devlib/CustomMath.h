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