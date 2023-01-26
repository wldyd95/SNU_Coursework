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


tinyfp fixp2tinyfp(fixp f)
{
	tinyfp result;
	// convert fixp f into float
	int b;
	float f_float = 0;
	//b31
	// if f >= 0, MSB is 0 and b is also 0000....0000 = 0
	// if f < 0, MSb is 1 and b is 1111....1111 = -1, so b is automatically negative (-1)
	b = (f >> 31);
	f_float += ((b) << 21);

	int i;
	// b30 ~ b10
	for(i = 30; i >= 10; i--){
		f = f - (b << (i+1));
		b = (f >> i);
		f_float += (b << (i-10));
	}

	// b9 ~ b0
	float n = (1.0);
	for(i = 9; i >= 0; i--){
		f = f - (b << (i+1));
		b = (f >> i);
		n *= 2;
		f_float += (b / n);
	}

	result = float2tinyfp(f_float);
	return result;
}


fixp tinyfp2fixp(tinyfp t)
{
	fixp result = 0;
	char bias = 3;

	// get sign bit of tinyfp t
	char sign;
	sign = t >> 7; // if t is positive, sign is 0 & if negative, sign is 1

	// get tinyfp except sign bit : (E2E1E0)(F0F1F2F3)
	t = t - (sign << 7); 

	// get each exponential & fraction bit
	char E2, E1, E0;
	char F3, F2, F1, F0;
	F3 = t % 2, t /= 2;
	F2 = t % 2, t /= 2;
	F1 = t % 2, t /= 2;
	F0 = t % 2, t /= 2;
	E0 = t % 2, t /= 2;
	E1 = t % 2, t /= 2;
	E2 = t % 2, t /= 2;

	char exponent = (E2 << 2) + (E1 << 1) + (E0 << 0);
	char E;

	// normalized value (1.F3F2F1F0 * 2^(exponent - bias))
	if(exponent!=0b000 && exponent!=0b111){
		E = exponent - bias;

		result += (1 << (10 + E));
		result += (F0 << (9 + E));
		result += (F1 << (8 + E));
		result += (F2 << (7 + E));
		result += (F3 << (6 + E));

		if(sign == 1) result = ~result +1; // 2s complement

		return result;
	}	
	
	// denormalized value (0.F3F2F1F0 * 2^(1 - bias))
	else if(exponent == 0b000){
		E = 1 - bias;
		result += (0 << (10 + E));
		result += (F0 << (9 + E));
		result += (F1 << (8 + E));
		result += (F2 << (7 + E));
		result += (F3 << (6 + E));

		if(sign == 1) result = ~result +1; // 2s complement

		return result;
	}
	
	// special case (infinity or NaN)
	else{
		return 0x80000000;
	}
}


tinyfp float2tinyfp(float f)
{	
	tinyfp result = 0;

	// special case test
	union _union{
		unsigned int i;
		float f;
		} u;

	u.f = f;

	if(u.i == 0x7F800000) return 0b01110000; // + infinity
	else if(u.i == 0xFF800000) return 0b11110000; // -infinity
	// caution : nan expression is not unique
	else if( ((u.i >> 23) & 0xff) == 0xff ){
		if( (u.i >> 31 ) == 1 ) return 0b11111100; // -NaN
		else return 0b01111100; // +NaN 
	}
	
	///////////////////////////////////////////////////////////////////////////

	// range check
	float tiny_max = (1 + 1/2.0 + 1/4.0 + 1/8.0 + 1/16.0 + 1/32.0) * (1 << 3);
	float tiny_min = (1 /32.0) / (1 << 2);

	int sign = 0;
	if(f < 0 || u.i == 0x80000000) { // caution : -0.0
		sign = 1;
		f = -f; // make negative f as positive
	}

	
	if(f <= tiny_min) { // if smaller than denormalized limit, truncate to 0
		if(sign == 1) return 0b10000000; // -0
		else return 0b00000000; // +0
	}

	else if(f >= tiny_max){ // if larger than normalized limit, truncate to infinity
		if(sign == 1) return 0b11110000; // - infinity
		else return 0b01110000; // + infinity
	}
	
	///////////////////////////////////////////////////////////////////////////
	// now f is in range of tinyfp
	int F0, F1, F2, F3, R; 
	int E2, E1, E0;
	float norm = 1.0; // (1.XXXX..) 
	float frac = 0.0; // (0.XXXX..)
	int E; // exponent - bias
	int exponent; 
	int bias = 3;

	// normalized case
	float norm_min = (1.0) / (1 << 2);
	if(f >= norm_min){ 
		norm = f;
		E = 0;

		if(f >= 1){
			while(norm >= 2){
				norm /= 2;
				E++;
			}
		}

		else{
			while(norm < 1){
				norm *= 2;
				E--;
			}
		}

		frac = (norm - 1);
		float temp = frac;

		temp *= 2, F0 = (int)temp, temp -= F0;
		temp *= 2, F1 = (int)temp, temp -= F1;
		temp *= 2, F2 = (int)temp, temp -= F2;
		temp *= 2, F3 = (int)temp, temp -= F3; // F3 is guard bit
		temp *= 2, R = (int)temp, temp -= R;

		// even rounding
		if(R == 1){
			if(frac > (F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + R/32.0)) frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + 2*(R/32.0);
			
			else if( frac == (F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + R/32.0) && F3 == 1) 	frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 
			+ 2*(R/32.0);
	
			else frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0;
		}

		else frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0;

		// get rounded value of f
		float f_round;
		if(E >= 0) f_round = (1.0 + frac) * (1 << E);
		else f_round = (1.0 + frac) / (1 << (-E));

		// recalculate by using f_round (for the case : eg. 1.11111 * 2^2 -> 1.0000 * 2^3)
		norm = f_round;
		E = 0;

		if(f_round >= 1){
			while(norm >= 2){
				norm /= 2;
				E++;
			}
		}

		else{
			while(norm < 1){
				norm *= 2;
				E--;
			}
		}

		frac = (norm - 1);
		temp = frac;

		temp *= 2, F0 = (int)temp, temp -= F0;
		temp *= 2, F1 = (int)temp, temp -= F1;
		temp *= 2, F2 = (int)temp, temp -= F2;
		temp *= 2, F3 = (int)temp, temp -= F3;

		exponent = E + bias;
		E0 = exponent % 2, exponent /= 2;
		E1 = exponent % 2, exponent /= 2;
		E2 = exponent % 2, exponent /= 2;

		result += (sign << 7);
		result += (E2 << 6);
		result += (E1 << 5);
		result += (E0 << 4);
		result += (F0 << 3);
		result += (F1 << 2);
		result += (F2 << 1);
		result += (F3 << 0);
	}  
	///////////////////////////////////////////////////////////////////////////

	// denormalized case (0.XXXX * 2^(-2))
	else{
		frac = f * (1 << (bias-1));
		float temp = frac;

		temp *= 2, F0 = (int)temp, temp -= F0;
		temp *= 2, F1 = (int)temp, temp -= F1;
		temp *= 2, F2 = (int)temp, temp -= F2;
		temp *= 2, F3 = (int)temp, temp -= F3; // F3 is guard bit
		temp *= 2, R = (int)temp, temp -= R;
		
		// even rounding
		if(R == 1){
			if(frac > (F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + R/32.0)) frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + 2*(R/32.0);
			
			else if( frac == (F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 + R/32.0) && F3 == 1) 	frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0 
			+ 2*(R/32.0);
	
			else frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0;
		}

		else frac = F0/2.0 + F1/4.0 + F2/8.0 + F3/16.0;

		// get rounded value of f
		float f_round;
		f_round = (0.0 + frac) / (1 << (bias-1));

		// recalculate by using f_round 
		frac = f_round * (1 << 2);
		if(frac == 1.0){ // caution!! : in this case, result becomes normalized value(eg. 0,11111 * 2^(-2) -> 1.00000 * 2^(-2))
			E2 = 0, E1 = 0, E0 = 1;
			F0 = 0, F1 = 0, F2 = 0, F3 = 0;
			result += (sign << 7);
			result += (E2 << 6);
			result += (E1 << 5);
			result += (E0 << 4);
			result += (F0 << 3);
			result += (F1 << 2);
			result += (F2 << 1);
			result += (F3 << 0);
		}

		else{
			float temp = frac;
			temp *= 2, F0 = (int)temp, temp -= F0;
			temp *= 2, F1 = (int)temp, temp -= F1;
			temp *= 2, F2 = (int)temp, temp -= F2;
			temp *= 2, F3 = (int)temp, temp -= F3;

			E2 = 0, E1 = 0, E0 = 0; // in denormalized case, exponent is 0
			result += (sign << 7);
			result += (E2 << 6);
			result += (E1 << 5);
			result += (E0 << 4);
			result += (F0 << 3);
			result += (F1 << 2);
			result += (F2 << 1);
			result += (F3 << 0);
		}
	}

	return result;

}


float tinyfp2float(tinyfp t)
{
	float result;
	char bias = 3;

	// get sign bit of tinyfp t
	char sign;
	sign = t >> 7; // if t is positive, sign is 0 & if negative, sign is 1

	// get tinyfp except sign bit : (E2E1E0)(F0F1F2F3)
	t = t - (sign << 7); 

	// get each exponential & fraction bit
	char E2, E1, E0;
	char F3, F2, F1, F0;
	F3 = t % 2, t /= 2;
	F2 = t % 2, t /= 2;
	F1 = t % 2, t /= 2;
	F0 = t % 2, t /= 2;
	E0 = t % 2, t /= 2;
	E1 = t % 2, t /= 2;
	E2 = t % 2, t /= 2;

	char exponent = (E2 << 2) + (E1 << 1) + (E0 << 0);
	float fraction = (F0)/2.0 + (F1)/4.0 + (F2)/8.0 + (F3)/16.0;
	char E;

	// normalized value (1.F3F2F1F0 * 2^(exponent - bias))
	if(exponent!=0b000 && exponent!=0b111){
		E = exponent - bias;

		if(E >= 0) result = (1*1 + fraction) * (1 << E);
		else result = (1*1 + fraction) / (1 << (-E));

		if(sign == 1) result = -result; // minus sign

		return result;
	}

	// denormalized value (0.F3F2F1F0 * 2^(1 - bias))
	else if(exponent == 0b000){
		E = 1 - bias;
		result = (0*1 + fraction) / (1 << (-E));

		if(sign == 1) result = -result; // 2s complement

		return result;
	}	

	// special case (infinity or NaN)
	else{
		union _union{
		unsigned int i;
		float f;
		} u;

		if(fraction == 0){ 
			if(sign == 1) { // - infinity
				u.i = 0xFF800000;
				return u.f; 
			}
			else { // + infinity
				u.i = 0x7F800000;
				return u.f; 
			}
		}

		else { 
			if(sign == 1){ // -NaN
				u.i = 0xFFC00000;
				return u.f;
			}
			else{ // +NaN
				u.i = 0x7FC00000;
				return u.f;
			}
		}
	}
}
