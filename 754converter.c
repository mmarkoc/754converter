#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/*
 * Simple IEEE 754 converter that converts given hexadecimal number
 * to the decimal number based on the given sizes of fraction and exponent bits.
 * I have used this to test my manual IEEE 754 calculations. 
 * 
 * Usage example :
 *   ./754converter 4 3 0x1E
 *    0x1e in binary	: 00011110
 *    Fraction bits are	: 1110
 *    Exponent bits are	: 001
 *    Sign bit is	: 0
 *    (-1)^0 * 1.875000 * 2^-2 =  0.468750
 *
 * In this example given number is 0x1E ( 0001 1110 binary ) with 4 fraction bits ( 1110 ),
 * 3 exponent bits ( 001 ) and 1 sign bit ( 0 ).  
 */
 
/* Various argument limits, feel free to modify based on your needs.
 * Default set to limits of 32 bit ( 1 signed bit, 8 exponent bits and 23 fraction bits ).*/
#define MAX_ARGUMENT_SIZE 4 
#define FRACTION_MIN 2 
#define FRACTION_MAX 23
#define EXPONENT_MIN 3 
#define EXPONENT_MAX 8 
#define SIGN_BIT 1 
#define DENORMALIZED 1 
#define NORMALIZED 1 
#define ARG_ERROR "Usage: 754converter <# of frac_bits> <# of exp_bits> <number in hex>"

/* Check for correct input by comparing the given fraction and exponent sizes with the ones
 * defined in as MIN and MAX. Return how many bits are allowed in the number user provided. */
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

/* Check if the number given by the user is larger than the combined given bit size ( sign bit + fraction + exponent). */ 
void check_hex_size(int number, int bit_size){ 
	unsigned long compare = (unsigned long)pow(2,bit_size+1);

        /* If number is bigger, error out and exit. */
	if( number > compare ){
		printf("Number %x exceeds maximum number of bits. Only %d allowed here.\n", number,bit_size + SIGN_BIT);
		exit(0);
	}
}

/* Bit mask is used to get fraction bits of the number. */ 
unsigned long fraction_mask(unsigned long number, int fraction_bits){
	unsigned long fraction = number&((1<<fraction_bits)-1);
	return fraction;
}	

/* Bit mask is used to get exponent bits of the number. */ 
unsigned long exponent_mask(unsigned long number, int exponent_bits,int fraction_bits){
	unsigned long exponent = number & (((1<<fraction_bits+exponent_bits)-1)^((1<<fraction_bits)-1));
	return exponent>>fraction_bits;
}	

/* Bit mask is used to get sign bit of the number. */ 
unsigned long sign_mask(unsigned long number, int exponent_bits,int fraction_bits){
	unsigned long sign = number & (unsigned long)pow(2,exponent_bits+fraction_bits);
	return sign>>(fraction_bits+exponent_bits);
}	

/* Main converter function goes through all of the steps of IEEE 754 Standard. */
void ieee_754_converter(unsigned long fraction,unsigned long exponent,int bias,int sign,float calculated_fraction,int exponent_bits){
	/*  If exponent bits are all 1 it is the case of either infinity or not a number. */
	if( exponent == ((unsigned long)pow(2,exponent_bits)-1) ){
		/* If fraction bits are all 0 it is the case of infinity. */
		if( fraction == 0 ){
			/* If sign bit is 1 it is negative infinity otherwise positive infinity. */
			if( sign == 1 )
				printf("-Infinity\n");
			else
				printf("+Infinity\n");
		}
		/* Otherwise it is not a number. */
		else
			printf("Not A Number.\n");
		exit(0);
	}

	/* Exponent and Mantissa. */
	int e;
	float m;
	/* If exponent bits are all 0 number is denormalized otherwise it is normalized. */
	if(exponent == 0){
		e = 1 - bias;
		m = calculated_fraction;
	}	
	else{
		e = exponent - bias; 
		m = 1 + calculated_fraction;
	}
	/* Calculate the float number and print it. Formula used is:
         * value = (-1)^sign * mantissa * 2^exponent */ 
	float number = pow(-1,sign) * m * pow(2,e);
	printf("(-1)^%i * %f * 2^%i =  ",sign, m, e);
	printf("%f\n",number);
}

/* For IEEE754 calculation fraction bits are needed as float type */
float calculate_fraction(unsigned long fraction,int fraction_bits){

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

/* Print given number in binary form */
void print_binary(int number, int bit_size){

	for( int i= bit_size-1; i>=0; --i){
		if(number & ( 1<<i )){
			printf("1");
		}
		else{
			printf("0");
		}
        }
}

int main(int argc, char ** argv){

	/* Check for the correct number of arguments */
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
	
	/* Make sure that the given hex is within the size limit */
	check_hex_size(number,frac_exp_inputcheck(fraction_bits,exponent_bits));
		
	/* Print number given by the user in binary form */
	printf("0x%x in binary\t: " , number);
	print_binary(number, (fraction_bits+exponent_bits+SIGN_BIT));
	printf("\n");

	/* Get the fraction part of the given number and print it in binary*/
	fraction = fraction_mask(number,fraction_bits);
	printf("Fraction bits are\t: ");
	print_binary(fraction, fraction_bits);
	printf("\n");
	/* Get the exponent part of the given number and print it in binary*/
	exponent = exponent_mask(number,exponent_bits,fraction_bits);
	printf("Exponent bits are\t: ");
	print_binary(exponent, exponent_bits);
	printf("\n");
	/* Get the signed bit of the number and print it in binary */
	sign = sign_mask(number,exponent_bits,fraction_bits);
	printf("Sign bit is\t\t: ");
	print_binary(sign, SIGN_BIT);
	printf("\n");
	/* Calculate bias */
	bias = (int)pow(2,(exponent_bits-1))-1;

	/* Run the main function */
	ieee_754_converter(fraction,exponent,bias,sign,calculate_fraction(fraction,fraction_bits),exponent_bits);
	return 0;
}
