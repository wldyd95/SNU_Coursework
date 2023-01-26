//--------------------------------------------------------------
// 
//  4190.308 Computer Architecture (Spring 2019)
//
//  Project #2: TinyFP Representation
//
//  April 4, 2019
//
//  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//--------------------------------------------------------------

#include <stdio.h>
#include "pa2.h"

#define BYTE_FORMAT 		"%c%c%c%c%c%c%c%c"
#define BYTE_TO_BITS(byte)  ((byte) & 0x80 ? '1' : '0'), \
  							((byte) & 0x40 ? '1' : '0'), \
  							((byte) & 0x20 ? '1' : '0'), \
  							((byte) & 0x10 ? '1' : '0'), \
  							((byte) & 0x08 ? '1' : '0'), \
  							((byte) & 0x04 ? '1' : '0'), \
  							((byte) & 0x02 ? '1' : '0'), \
  							((byte) & 0x01 ? '1' : '0') 

#define PRINT_WORD(s,i,p)	printf(s"("BYTE_FORMAT" ", BYTE_TO_BITS((i) >> 24));	\
							printf(BYTE_FORMAT" ", BYTE_TO_BITS((i) >> 16)); 		\
							printf(BYTE_FORMAT" ", BYTE_TO_BITS((i) >> 8)); 		\
							printf(BYTE_FORMAT")"p, BYTE_TO_BITS(i));	
#define PRINT_FIXP(i,p)		PRINT_WORD("fixp", i, p)
#define PRINT_FLOAT(i,p)	PRINT_WORD("float", i, p)
#define PRINT_TINYFP(t,p)	printf("tinyfp("BYTE_FORMAT")"p, BYTE_TO_BITS(t));

#define TINYFP_INF_NAN(t)	((((t) >> 4) & 0x07) == 0x07)
#define FLOAT_INF_NAN(f)	((((f) >> 23) & 0xff) == 0xff)
#define TEST_INF_NAN(f,t)	(TINYFP_INF_NAN(t) && FLOAT_INF_NAN(f) && 				\
							((((t) >> 7) == (f) >> 31) && 							\
							(((((t) & 0x0f) == 0) && (((f) & 0x007fffff) == 0)) || 	\
							((((t) & 0x0f) > 0) && (((f) & 0x007fffff) > 0)))))

#define CHECK_VALUE(r,a)	printf("%s\n", ((r) == (a))? "CORRECT" : "WRONG")
#define CHECK_INF_NAN(f,t)	printf("%s\n", TEST_INF_NAN(f,t)? "CORRECT" : "WRONG")

#define N 	6

fixp fixp_literal[N] 		= {0x00000400, 0x00003000, 0x00000100, 0x00000080, 0x000003f0, 0xffffffff}; 
tinyfp fixp2tinyfpAnswer[N] = {0b00110000, 0b01101000, 0b00010000, 0b00001000, 0b00110000, 0b10000000}; 

tinyfp tinyfp_literal[N] 	= {0b10000000, 0b10001101, 0b10110000, 0b01010101, 0b01110000, 0b11111111};
fixp tinyfp2fixpAnswer[N] 	= {0x00000000, 0xffffff30, 0xfffffc00, 0x00001500, 0x80000000, 0x80000000};

float float_literal[N] 		 = {0.015625,   0.01171875,  -5.0,      15.625 ,    15.75,     -0.0/0.0};
tinyfp float2tinyfpAnswer[N] = {0b00000001, 0b00000001, 0b11010100, 0b01101111, 0b01110000, 0b11110001};

tinyfp tinyfp_literal2[N] 	= {0b00000010, 0b00100000, 0b11101010, 0b10000000, 0b01110000, 0b11111100};
float tinyfp2floatAnswer[N] = {0.03125,    0.5,        -13.0,      -0.0,       1.0/0.0,    -0.0/0.0}; 

int main(void)
{
	int i;
	tinyfp tf;
	fixp x;
	union _union{
		unsigned int i;
		float f;
	} u;

	printf ("\nTest 1: Casting from fixp to tinyfp\n");
	for (i = 0; i < N; i++)
	{
		x = fixp_literal[i];
		tf = fixp2tinyfp(x);
		PRINT_FIXP(x, " => ");
		PRINT_TINYFP(tf, ", ");
		CHECK_VALUE(tf, fixp2tinyfpAnswer[i]);
	}

	printf ("\nTest 2: Casting from tinyfp to fixp\n");
	for (i = 0; i < N; i++)
	{
		tf = tinyfp_literal[i];
		x = tinyfp2fixp(tf);
		PRINT_TINYFP(tf, " => ");
		PRINT_FIXP(x, ", ");
		CHECK_VALUE(x, tinyfp2fixpAnswer[i]);
	}

	printf ("\nTest 3: Casting from float to tinyfp\n");
	for (i = 0; i < N; i++)
	{
		u.f = float_literal[i];
		tf = float2tinyfp(u.f);
		PRINT_FLOAT(u.i, " => ");
		PRINT_TINYFP(tf, ", ");
		FLOAT_INF_NAN(u.i)? CHECK_INF_NAN(u.i, tf) : CHECK_VALUE(tf, float2tinyfpAnswer[i]);
	}

	printf ("\nTest 4: Casting from tinyfp to float\n");
	for(i = 0; i < N; i++)
	{
		tf = tinyfp_literal2[i];
		u.f = tinyfp2float(tf);
		PRINT_TINYFP(tf, " => ");
		PRINT_FLOAT(u.i, ", ");
		TINYFP_INF_NAN(tf)? CHECK_INF_NAN(u.i, tf) : CHECK_VALUE(u.f, tinyfp2floatAnswer[i]);
	}

	return 0;
}
