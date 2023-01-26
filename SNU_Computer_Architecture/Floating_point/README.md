# 4190.308 Computer Architecture (Spring 2019)
# Project #2: TinyFP (8-bit floating point) representation
### Due: 11:59PM (Sunday), April 14


## Introduction

The purpose of this project is to get familiar with the floating-point representation by implementing a simplified 8-bit floating-point representation.

## Problem specification

`tinyfp` is a simplified 8-bit floating-point representation that follows the IEEE 754 standard for floating-point arithmetic. The overall structure of the `tinyfp` representation is shown below. The MSB (Most Significant Bit) is used as a sign bit (`S`). The next three bits are used for exponent (`E`) with a bias value of 3. The last four bits are used for the fractional part (`F`).

```
bit 7  6  5  4  3  2  1  0
    S  E  E  E  F  F  F  F
    ^  +--+--+  +---+----+
    |     |         |                       
Sign bit  |       Fraction (4 bits)
         Exponent (3 bits)                   
```

In C, the new type `tinyfp` is defined as follows.

```
typedef unsigned char tinyfp;
```

Your task is to implement the following four C functions that convert `fixp` or `float` type values to the `tinyfp` format and vice versa. The `fixp` type indicates the 21.10 fixed-point representation that was defined in the previous Project #1.

```
tinyfp fixp2tinyfp (fixp f);        // convert fixp -> tinyfp
fixp tinyfp2fixp (tinyfp t);        // convert tinyfp -> fixp
tinyfp float2tinyfp (float f);      // convert float -> tinyfp
float tinyfp2float (tinyfp t);      // convert tinyfp -> float
```

## Implementation details

### fixp2tinyfp()

 * Not all the `fixp`-type values can be represented in the `tinyfp` format because `tinyfp` has only four bits of fraction and a limited range. When necessary, you should use the round-to-even rounding mode.
 * The value 0.0 in `fixp` should be converted to plus zero (+0.0) in `tinyfp`.
 * Any `fixp`-type value that exceeds the range of the `tinyfp` representation (after rounding) should be converted to the infinity in `tinyfp` (+&infin; or -&infin; depending on the sign).

### tinyfp2fixp()
 * The smallest positive number in the `tinyfp` format is 0.0001 x 2<sup>-2</sup> and the largest positive number in `tinyfp` is 1.1111 x 2<sup>3</sup>, both of which can be correctly represented as the 21.10 fixed-point format. Therefore, any number in the `tinyfp` format can be converted to the `fixp` format without any error.
 * Both of -0.0 and +0.0 in `tinyfp` should be converted to the value 0.0 in `fixp`.
 * Convert +&infin; and -&infin; in `tinyfp` to the smallest number in `fixp` (i.e., 0x80000000).
 * +NaN and -NaN in `tinyfp` are also converted to the smallest number in `fixp` (i.e., 0x80000000).

### float2tinyfp()
 * Not all the `float`-type values can be represented in the `tinyfp` format. When necessary, you should use the round-to-even rounding mode.
 * Any floating-point value that exceeds the range of the `tinyfp` representation (after rounding) should be converted to the inifinity in `tinyfp` (+&infin; or -&infin; depending on the sign).
 * +&infin; and -&infin; in `float` should be converted to the corresponding +&infin; and -&infin; in `tinyfp`, respectively.
 * +NaN and -NaN in `float` should be converted to the corresponding +NaN and -NaN in `tinyfp`, respectively.

### tinyfp2float()
 * The `tinyfp` type is a subset of the `float` type. Hence, all the values in `tinyfp` can be converted in the `float` format without any error.
 * Again, +&infin; and -&infin; in `tinyfp` should be converted to the corresponding +&infin; and -&infin; in `float`, respectively. +NaN and -NaN in `tinyfp` should be converted to the corresponding +NaN and -NaN in `float`, respectively.


## Skeleton code

We provide you with the skeleton code for this project. It can be download from Github at https://github.com/snu-csl/ca-pa2/. If you don't have the git utility, you need to install it first by performing "`sudo apt install git`" command. To download and build the skeleton code, please follow these steps:

```
$ git clone https://github.com/snu-csl/ca-pa2.git
$ cd ca-pa2
$ make
gcc -g -O2 -Wall  -c pa2-test.c -o pa2-test.o
gcc -g -O2 -Wall  -c pa2.c -o pa2.o
gcc -g -O2 -Wall  -o pa2 pa2-test.o pa2.o
```

The result of a sample run looks like this:

```
$ ./pa2
Test 1: Casting from fixp to tinyfp
fixp(00000000 00000000 00000100 00000000) => tinyfp(00001001), WRONG
fixp(00000000 00000000 00110000 00000000) => tinyfp(00001001), WRONG
fixp(00000000 00000000 00000001 00000000) => tinyfp(00001001), WRONG
fixp(00000000 00000000 00000000 10000000) => tinyfp(00001001), WRONG
fixp(00000000 00000000 00000011 11110000) => tinyfp(00001001), WRONG
fixp(11111111 11111111 11111111 11111111) => tinyfp(00001001), WRONG

Test 2: Casting from tinyfp to fixp
tinyfp(10000000) => fixp(00000000 00000000 00000000 00001001), WRONG
tinyfp(10001101) => fixp(00000000 00000000 00000000 00001001), WRONG
tinyfp(10110000) => fixp(00000000 00000000 00000000 00001001), WRONG
tinyfp(01010101) => fixp(00000000 00000000 00000000 00001001), WRONG
tinyfp(01110000) => fixp(00000000 00000000 00000000 00001001), WRONG
tinyfp(11111111) => fixp(00000000 00000000 00000000 00001001), WRONG

Test 3: Casting from float to tinyfp
float(00111100 10000000 00000000 00000000) => tinyfp(00001001), WRONG
float(00111100 01000000 00000000 00000000) => tinyfp(00001001), WRONG
float(11000000 10100000 00000000 00000000) => tinyfp(00001001), WRONG
float(01000001 01111010 00000000 00000000) => tinyfp(00001001), WRONG
float(01000001 01111100 00000000 00000000) => tinyfp(00001001), WRONG
float(11111111 11000000 00000000 00000000) => tinyfp(00001001), WRONG

Test 4: Casting from tinyfp to float
tinyfp(00000010) => float(01000001 00011110 01100110 01100110), WRONG
tinyfp(00100000) => float(01000001 00011110 01100110 01100110), WRONG
tinyfp(11101010) => float(01000001 00011110 01100110 01100110), WRONG
tinyfp(10000000) => float(01000001 00011110 01100110 01100110), WRONG
tinyfp(01110000) => float(01000001 00011110 01100110 01100110), WRONG
tinyfp(11111100) => float(01000001 00011110 01100110 01100110), WRONG
```

You are required to complete the `fixp2tinyfp()`, `tinyfp2fixp()`, `float2tinyfp()`, and `tinyfp2float()` functions in the `pa2.c` file.


## Restrictions

* You should not use any array even in comment lines. Any source file that contains the symbol [ or ] will be rejected by the server.

* Do not use the `double` type. Any source file that contains the word `double` (even in comment lines) will be rejected.

* Do not use any header file other than `stdio.h` and `pa2.h` in the `pa2.c` file.



## Hand in instructions

* Submit only the `pa2.c` file to the submission server.

## Logistics

* You will work on this project alone.
* Only the upload submitted before the deadline will receive the full credit. 25% of the credit will be deducted for every single day delay.
* __You can use up to 5 _slip days_ during this semester__. If your submission is delayed by 1 day and if you decided to use 1 slip day, there will be no penalty. In this case, you should explicitly declare the number of slip days you want to use in the QnA board of the submission server after each submission. Saving the slip days for later projects is highly recommended!
* Any attempt to copy others' work will result in heavy penalty (for both the copier and the originator). Don't take a risk.

Have fun!

[Jin-Soo Kim](mailto:jinsoo.kim_AT_snu.ac.kr)  
[Systems Software and Architecture Laboratory](http://csl.snu.ac.kr)  
[Dept. of Computer Science and Engineering](http://cse.snu.ac.kr)  
[Seoul National University](http://www.snu.ac.kr)
