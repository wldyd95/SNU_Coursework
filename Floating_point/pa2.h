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

#ifndef _PA_2_H_
#define _PA_2_H_

typedef unsigned char tinyfp;	// 8-bit floating-point
typedef int fixp;				// 21.10 fixed-point

// Convert fixp to tinyfp
tinyfp fixp2tinyfp (fixp f);

// Convert tinyfp to fixp
fixp tinyfp2fixp(tinyfp t);

// Convert float to tinyfp
tinyfp float2tinyfp(float f);

// Convert tinyfp to float
float tinyfp2float(tinyfp t);

#endif // _PA_2_H_
