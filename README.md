# 754converter
Simple IEEE 754 hexadecimal to decimal converter. For one of my classes I had to 
do a lot of manual IEEE 754 calculations with sub 32 bit numbers. All of the
existing converters/calculators I found only supported single precision 
(1 sign bit + 8 exponents bits + 23 fraction bits) so I wrote the converter that
allows you to set the fraction and exponent size.

## Usage
`754converter <# of frac_bits> <# of exp_bits> <number in hex>`
