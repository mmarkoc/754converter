#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/*
 * Simple IEEE 754 converter that converts given hexadecimal number
 * to the decimal number based on the given sizes of fraction and exponent bits.
 * I have used this to test my manual IEEE 754 calculations.
 */
 
// Various argument limits, feel free to modify based on your needs.
// Default set to limits of 32 bit ( 1 signed bit, 8 exponent bits and 23 fraction bits ).
#define MAX_ARGUMENT_SIZE 4 
#define FRACTION_MIN 2 
#define FRACTION_MAX 23
#define EXPONENT_MIN 3 
#define EXPONENT_MAX 8 
#define SIGNED_BIT 1 
#define DENORMALIZED 1 
#define NORMALIZED 1 
#define ARG_ERROR "Usage: 754converter <# of frac_bits> <# of exp_bits> <number in hex>"

// Check for correct input, it returns how many bits is allowed in number
int frac_exp_inputcheck(int fraction,int exponent){

	if( fraction < FRACTION_MIN || fraction > FRACTION_MAX ){
		printf("Invalid number of fraction bits (%i). Should be between %i and %i.\n", fraction, FRACTION_MIN, FRACTION_MAX);
		exit(0);
	}	

	if( exponent < EXPONENT_MIN || exponent > EXPONENT_MAX ){
		printf("Invalid number of exponent bits (%i). Should be between %i and %i.\n", exponent, EXPONENT_MIN, EXPONENT_MAX);
		exit(0);
	}	

	return fraction + exponent;
}

// Check if the hex number provided exceeds the allowed bit size ( signed bit + fraction + exponent )  
void check_hex_size(int number, int bit_size){

	unsigned long compare = (unsigned long)pow(2,bit_size+1);

	// Notify and exit if provided number is larger than given bit size
	if( number > compare ){
		printf("Number %x exceeds maximum number of bits. Only %d allowed here.\n", number,bit_size + SIGNED_BIT);
		exit(0);
	}
}

// Fraction parser
unsigned long fractionParser(unsigned long number, int fraction_bits){
	unsigned long fraction = number&((1<<fraction_bits)-1);
	return fraction;
}	

// Exponent parser
unsigned long exponentParser(unsigned long number, int exponent_bits,int fraction_bits){
	unsigned long exponent = number & (((1<<fraction_bits+exponent_bits)-1)^((1<<fraction_bits)-1));
	return exponent>>fraction_bits;
}	

// Sign bit parser
unsigned long signParser(unsigned long number, int exponent_bits,int fraction_bits){
	unsigned long sign = number & (unsigned long)pow(2,exponent_bits+fraction_bits);
	return sign>>(fraction_bits+exponent_bits);
}	

// Our main calculator function
void ieee754Converter(unsigned long fraction,unsigned long exponent,int bias,int sign,float calculated_fraction,int exponent_bits){
	// If exponent is all 1
	if(exponent == ((unsigned long)pow(2,exponent_bits)-1)){
		// If fraction is all 0
		if( fraction == 0 ){
			// Infinity
			if(sign == 1)
				printf("-inf\n");
			else
				printf("+inf\n");
		}
		// NaN
		else
			printf("NaN\n");
		
		exit(0);
	}

	// E and M
	int e;
	float m;
	// Denormalized
	if(exponent == 0){
		e = 1 - bias;
		m = calculated_fraction;
	}	
	// Normalized
	else{
		e = exponent - bias; 
		m = 1 + calculated_fraction;
	}
	// Calculate the float number and print it
	float number = pow(-1,sign) * m * pow(2,e);
	printf("%f\n",number);
}

// Return fraction in decimal value
float calculateFraction(unsigned long fraction,int fraction_bits){

	int i = fraction_bits-1;
	int j = 1;
	float calculated_fraction = 0;

	for (i;i>=0;--i){
		if( (fraction & ( 1<<i )) != 0 ){
			calculated_fraction += 1/pow(2,j);
		}
		++j;
	}

	return calculated_fraction;

}

int main(int argc, char ** argv){

	// Check for the correct number of arguments
	if( argc != MAX_ARGUMENT_SIZE){
		printf("%s\n",ARG_ERROR);
		exit(0);
	}

	int fraction_bits;
	int exponent_bits;
	int number;
	char extra[20];
	unsigned long fraction;
	unsigned long exponent;
	float exponent_calculated;
	int sign;
	int bias;

	// Check for sscanf success
	if( 1 != sscanf(argv[1], "%d", &fraction_bits) ||
	    1 != sscanf(argv[2], "%d", &exponent_bits) ||
	    2 == sscanf(argv[3], "%x%s", &number, extra)  ||
	    1 != sscanf(argv[3], "%x", &number) ){
	    	if ( 2 == sscanf(argv[3], "%x%s", &number,extra) || 1 != sscanf(argv[3], "%x", &number)) 
			printf("Argument 3 is not a hexadecimal number (%s).\n", argv[3]);
		else
			printf("ERROR\n");
		exit(0);
	}
	
	// Make sure that the given hex is within the size limit
	check_hex_size(number,frac_exp_inputcheck(fraction_bits,exponent_bits));
	
	// Get the fraction part of the given number
	fraction = fractionParser(number,fraction_bits);
	// Get the exponent part of the given number
	exponent = exponentParser(number,exponent_bits,fraction_bits);
	// Get the signed bit of the number
	sign = signParser(number,exponent_bits,fraction_bits);
	// Calculate bias
	bias = (int)pow(2,(exponent_bits-1))-1;

	// Run the main function
	ieee754Converter(fraction,exponent,bias,sign,calculateFraction(fraction,fraction_bits),exponent_bits);
	return 0;
}
