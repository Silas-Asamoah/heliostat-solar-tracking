/*
	Adapted from:
	- libmath.b
	bc-1.06 
	13th September 2000
	
	- BigNumberMathFunctions.ino
	Mega_SunTracker_Heliostat_Program_V097_b @ cerebralmeltdown.com
	5th July 2012
	
	Original Authors: Philip A. Nelson & Gabriel
	Library Adaptation: sj
	v3
*/


// WARNING:
// - dividing a BigNumber reduces the scale to the current scale
// - all non-integer constants are defined with a larger scale (use pi/one, piOverTwo/one etc...)
// - cosine is now quite a bit slower than sine, this is to maintain accuracy
// - order of arctan2 arguments reversed compared to previous versions, specifically, arctan2(x,y)=arctan(x+i*y)
// - arctan2(0,0) is indeterminate: we INCORRECTLY return 0
// - arcsin(z) on z \notin [-1,1] is complex: we INCORRECTLY return 0
// - BigNumber.pow() on numbers less than 1 can be slower than raiseToPower

// CHANGES:
// v3
// - removed precision specifications, all functions are now precise to all but the last digit
// - introduced getScale which returns the current BigNumber scale
// - introduced setScale to change scale of calculation without reinitialising
// - introduced default scale = 0
// - deg, rad conversion now performed by piOverOneEighty (saves a division)
// - optimised sine, cosine, arcsine, arctan2, arctan
// - removed all non-trigonometric functions except raiseToPower
// v2
// - introduced constant piOverTwo = pi/2
// - implemented finish() which cleanly removes BigNumberMath from memory
// - checked for memory leaks using "MemoryFree" library (no further changes)
// v1
// - implemented arctan(z), arcsine(z), arctan2(x,y)
// - changed type of precision to int (be realistic)
// - changed argument of factorial to int (be realisitic)
// - reduced number of BigNumbers in memory (mostly using a = f(a) rather than b = f(a))

// TODO: 
// - make quick sign flips possible: x = -x (using bc_num x->n_sign)
// - use instead number.c's _zero_, _one_, _two_ as constants

#include <BigNumberMath.h>

BigNumber BigNumberMath::pi;
BigNumber BigNumberMath::piOverOneEighty;
BigNumber BigNumberMath::piOverTwo;
BigNumber BigNumberMath::piOverFour;
BigNumber BigNumberMath::piOverSix;
BigNumber BigNumberMath::rootThree;
BigNumber BigNumberMath::twoPlusRootThree;
BigNumber BigNumberMath::twoMinusRootThree;
BigNumber BigNumberMath::recipRootThree;
BigNumber BigNumberMath::minusOne;
BigNumber BigNumberMath::zero;
BigNumber BigNumberMath::one;
BigNumber BigNumberMath::two;
BigNumber BigNumberMath::three;

// initialise package
void BigNumberMath::begin(const int scale)
{
	BigNumber::begin (scale);
	
	// defaults are valid to 50 decimals
    setScale(scale+4);
	pi                = BigNumber("3.14159265358979323846264338327950288419716939937511");
	piOverOneEighty   = BigNumber("0.01745329251994329576923690768488612713442871888542");
	setScale(scale*1.2); // need piOverTwo at higher scale
    piOverTwo         = BigNumber("1.57079632679489661923132169163975144209858469968755");
	setScale(1.1*scale+2); // need piOverFour at higher scale
	piOverFour        = BigNumber("0.78539816339744830961566084581987572104929234984378");
	setScale(scale+5); // need piOverSix at higher scale
    piOverSix         = BigNumber("0.52359877559829887307710723054658381403286156656252");
    setScale(scale+3); // need recipRootThree at higher scale
	rootThree         = BigNumber("1.73205080756887729352744634150587236694280525381038");
	twoPlusRootThree  = BigNumber("3.73205080756887729352744634150587236694280525381038");
	twoMinusRootThree = BigNumber("0.26794919243112270647255365849412763305719474618962");
	recipRootThree    = BigNumber("0.57735026918962576450914878050195745564760175127013");
	setScale(0);
	minusOne          = BigNumber(-1);
	zero              = BigNumber(0);
	one               = BigNumber(1);
	two               = BigNumber(2);
	three             = BigNumber(3);
	setScale(scale);
}

// exit BigNumberMath cleanly
void BigNumberMath::finish()
{
    // Explicitly destruct constants
	pi.~BigNumber();
    piOverOneEighty.~BigNumber();
    piOverTwo.~BigNumber();
    piOverFour.~BigNumber();
    piOverSix.~BigNumber();
    rootThree.~BigNumber();
	twoPlusRootThree.~BigNumber();
	twoMinusRootThree.~BigNumber();
    recipRootThree.~BigNumber();
    minusOne.~BigNumber();
    zero.~BigNumber();
    one.~BigNumber();
    two.~BigNumber();
    three.~BigNumber();
  
    // Finish BigNumber
    BigNumber::finish();
}

// set scale
void BigNumberMath::setScale(const int scale) {
	BigNumber::setScale(scale);
}

// get scale
int BigNumberMath::getScale() {
    int a = BigNumber::setScale();
	BigNumber::setScale(a);
	return a;
}

// == BOF: TRIGONOMETRIC FUNCTIONS ==
// cos(z) on z \elem [-Infinity,Infinity]
BigNumber BigNumberMath::cosine(BigNumber z)
{
    int scale;
    scale = getScale();
    setScale(scale*1.2);
    z = sine(z+piOverTwo);
    setScale(scale);
    return (z/one);
}

// sin(z) on z \elem [-Infinity,Infinity]
// Taylor series
BigNumber BigNumberMath::sine(BigNumber z)
{
    int scale;
	BigNumber n;
    BigNumber v,s,e;
    bool m = false; // m= false => z positive, m= true => z negative)
  
    scale = getScale();
  
    if (z < zero) {
        m = true;
        z = minusOne*z;
    }
  
    // range reduction to (-Pi/2,Pi/2)
    setScale(0);
    n = (z/piOverFour+two)/BigNumber(4);
    z -= pi*n;
    if (int(n%two)) z = minusOne*z;
  
    // taylor series
    setScale(scale+2);
    v = z;
    e = z;
    s = minusOne*z*z;
    for (int i=3; 1; i+=2) {
        e *= s/BigNumber((i*(i-1)));
        if (e == zero) {
            setScale(scale);
            if (m) return (v/minusOne);
            return (v/one);
        }
        v += e;
    }
}

// Pr arcsin(z) on z \elem [-1,1]
BigNumber BigNumberMath::arcsine(BigNumber z)
{
	// Complex BigNumbers not implemented
	if(z>one) return zero;
	if(z<minusOne) return zero;
	// Special cases
	if(z==one) return piOverTwo/one;
	if(z==minusOne) return piOverTwo/minusOne;
	
	int scale = getScale();
	setScale(scale*1.2);
	z=z/(one-z*z).sqrt();
	setScale(scale);
	
	return arctangent(z/one);
}

// Pr arg(x+i*y)
// Floating point implementation
BigNumber BigNumberMath::arctan2(BigNumber x, BigNumber y)
{
    // Special cases
	if(y.isZero() && x.isZero()) return zero; // undefined
	if(x.isZero() && y<zero) return piOverTwo/minusOne;
	if(x.isZero() && y>zero) return piOverTwo/one;	
	if(y<zero && x<zero) return arctangent(y/x)-pi/one;
	if(x<zero) return arctangent(y/x)+pi/one; // y>=zero
	return arctangent(y/x); // x>zero
}

// Pr arctan(z) on z \elem [-Infinity,Infinity]
// Taylor series
BigNumber BigNumberMath::arctangent(BigNumber z) {
    BigNumber f,n,s,e,v;
    int scale;
    bool m = false;   // m= false => z positive, m= true => z negative)
  
    // negative z
    if (z<zero) {
        m = true;
        z = minusOne*z;
    }

    /*
    // special case and for fast answers
    if (z==one) { // ArcTan[1] = Pi/4
    	if(m) return piOverFour/minusOne;
	    return piOverFour/one;
    }
    if (z==recipRootThree) { // ArcTan[1/Sqrt[3]] = Pi/6
	    if(m) return piOverSix/minusOne;
	    return piOverSix/one;
    }
    */  
  
    scale = getScale();
  
    // range reduction to (-2+Sqrt[3],2-Sqrt[3])
    // optimised repeated application of (z-1/Sqrt[3])/(1+z*1/Sqrt[3])
    setScale(scale+3);
    if (z > twoPlusRootThree) {
        f = three;
	    z = minusOne/z;
    }
    else if (z > one) {
        f = two;
	    z = (z-rootThree)/(one+rootThree*z);
    } 
    else if (z > twoMinusRootThree) {
        f = one;
	    z = (three*z-rootThree)/(three+rootThree*z);
    }
    else { // else no range reduction necessary 
        f = zero;
    }
	
    // taylor series
    v = z;
    n = z;
    s = minusOne*z*z;
    for (int i=3; 1; i+=2) {
        n *= s;
        e = n/BigNumber(i);
        if (e == zero) {
            setScale(scale);
	        if(m) return (f*piOverSix+v)/minusOne;
            return (f*piOverSix+v)/one;
        }
        v += e;
    }
}

// degrees to radians
BigNumber BigNumberMath::to_BigRad(BigNumber angle)
{
    return (angle*piOverOneEighty)/one;
}

// radians to degrees
BigNumber BigNumberMath::to_BigDeg(BigNumber angle)
{
    return (angle/piOverOneEighty)/one;
}
// == EOF: TRIGONOMETRIC FUNCTIONS ==

// == BOF: BASIC FUNCTIONS ==

// base^exponent  
BigNumber BigNumberMath::raiseToPower(BigNumber base, long exponent)
{
    BigNumber product=1;
    for (long n = 1; n <= exponent; n++){
        product=product*base;
    }
    return product;
}

/*
// Reduce scale of variable to current scale (with correct rounding)
BigNumber BigNumberMath::roundToScale(BigNumber z)
{
	int scale;
	int decimal;
	char * str;
	BigNumber x;
	
	// Get current scale
	scale = getScale();
	
	//printBigNum(z); Serial.println("");
	
	x /= one; // z at current scale
	
	//printBigNum(x); Serial.println("");
	
	// Horrible hack, better would be to directly check scale+1 digit and round up if necessary
	// determine if scale+1 decimal is > 5 (z=0 if not)
	setScale(scale+1);
	z = (z - x)/BigNumber(5);
	z /= one; // z at current scale
	z *= BigNumber(10);
	
	//printBigNum(z); Serial.println("");
	
	setScale(scale);
	// if scale+1 decimal is > 5 round up
	if(!z.isZero()) {  
	    z /= one; // z at current scale
		if (!z.isZero()) x = x + z;
	}
	return x;
}
*/

/*
// function to display a big number and free it afterwards
// modified println to print
void BigNumberMath::printBigNum (BigNumber n)
{
  char * s = n.toString ();
  Serial.print (s);
  free (s);
}  // end of printBignum
*/

// == EOF: BASIC FUNCTIONS ==