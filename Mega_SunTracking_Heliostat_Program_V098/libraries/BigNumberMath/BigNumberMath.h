/* 
	BigNumberMath.h - Library implementing some useful 
	mathematical functions with arbitrary precision
	
	REQUIRES BigNumber.h
	BigNumber.h must be included in sketch
*/
#ifndef BigNumberMath_h
#define BigNumberMath_h

#include <Arduino.h>
#include <BigNumber.h>

class BigNumberMath
{
    public:
		// INITIALISE
		static void begin(const int scale = 0);
		
		// TERMINATE
		static void finish();
		
		// LIBRARY FUNCTIONS
		static void setScale(const int scale = 0);
		static int getScale();
		
		// CONSTANTS
		static BigNumber pi;
		static BigNumber piOverOneEighty;
		static BigNumber piOverTwo;
		static BigNumber piOverFour;
		static BigNumber piOverSix;
		static BigNumber rootThree;
		static BigNumber twoPlusRootThree;
		static BigNumber twoMinusRootThree;
		static BigNumber recipRootThree;
		static BigNumber minusOne;
		static BigNumber zero;
		static BigNumber one;
		static BigNumber two;
		static BigNumber three;
		
		// TRIGONOMETRIC FUNCTIONS
		static BigNumber cosine(BigNumber z);
		static BigNumber sine(BigNumber z);
		static BigNumber arcsine(BigNumber z);
		static BigNumber arctan2(BigNumber y, BigNumber x);
		static BigNumber arctangent(BigNumber z);
		static BigNumber to_BigRad(BigNumber angle);
		static BigNumber to_BigDeg(BigNumber angle);
		
		// BASIC FUNCTIONS
		static BigNumber raiseToPower(BigNumber base, long exponent);
		//static BigNumber roundToScale(BigNumber z);
		//static void printBigNum(BigNumber n);

};

#endif