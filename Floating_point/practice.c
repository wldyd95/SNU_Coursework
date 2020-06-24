#include <stdio.h>

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

char float2tinyfp(float f)
{	
	char result = 0;

	// special case test
	union _union{
		unsigned int i;
		float f;
		} u;

	u.f = f;
	
	if(u.i == 0x7F800000) return 0b01110000; // + infinity
	else if(u.i == 0xFF800000) return 0b11110000; // -infinity
	else if(u.i == 0xFFC00000) return 0b11111100; // -NaN
	else if(u.i == 0x7FC00000) return 0b01111100; // +NaN
	
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
	float norm = 1; // (1.XXXX..) 
	float frac = 0; // (0.XXXX..)
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

unsigned char test(){
	return 0b11110000;
}


int main(){

    union _union{
		unsigned int i;
		float f;
	} u;
	float a = -(1/2.0 + 1/4.0 + 1/8.0 + 1/16.0 + 1/32.0)/4;
	//PRINT_FLOAT(u.i, "\n");
	unsigned char c = float2tinyfp(a);
	printf("%f\n", a);
	PRINT_TINYFP(c, "\n");
    return 0;
}