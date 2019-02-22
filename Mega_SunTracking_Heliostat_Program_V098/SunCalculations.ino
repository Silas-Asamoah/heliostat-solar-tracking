
void findSunsAltAndAzOne(int year, int month, int day, int timezone, int hour, int minute, int second, float latitude, float longitude){
      
        BigNumberMath::begin (calculationScale);  // initialize library // sj
            
         BigNumber jd = findjd(year, month, day, timezone, hour, minute, second);              
        //printBignum (jd);
                 
         BigNumber t = TimeInJulianCenturies(jd);
        //printBignum (t);  
        
         BigNumber l = MeanEquinoxOfTheDate(t);
         //printBignum (l);  

         BigNumber m = MeanAnomalyOfTheSun(t);
         //printBignum (m);

         BigNumber c = SunsEquationOfTheCenter(m, t);
         //printBignum (c);            

         BigNumber o = l + c;//Sun's True Longitude
         //printBignum (o);            

         BigNumber lambda = SunsApparantLongitude(t, o);
         //printBignum (lambda);
        
         BigNumber epsilon = ObliquityOfTheEcliptic(t);
         //printBignum (epsilon);
      
         BigNumber delta = SunsDeclination(epsilon, lambda);
         //printBignum (delta);     

         BigNumber alpha = SunsRightAscension( epsilon, lambda);
         //printBignum (alpha);     

         BigNumber theta = SiderealTime(t, jd);
         //printBignum(theta);        
 
         BigNumber h = LocalHourAngle(theta, alpha, longitude);
         //printBignum(h);                        
         
         findAlt(latitude, delta, h);//Note: altitude is a global variable.
         findAz(latitude, delta, h);//Note: azimuth is a global variable.
       
         BigNumberMath::finish (); // sj

}

BigNumber findjd(int year, int month, int day, int timezone, int hour, int minute, int second){     
  
        BigNumber v1,v2;
        char buf1[15];
	BigNumber d = BigNumber(day) + ((BigNumber(hour + (-1.0 * timezone)))/"24")
                      + BigNumber(minute)/"1440" + BigNumber(second)/"86400";
         
	 if (month > 2){
         }else{
	  year = year-1;
	  month = month+12;
         }
	 float a = (float)((long)(((float)year)/100.0));
         float b = 2 - a + (long)(a/4);
         v1=dtostrf(b,5,5,buf1);

         long x = (long)(365.25 * (year + 4716.0)) + (long)(30.6001 * (month+1));
         v2 = ltoa(x, buf1, 10);
         BigNumber jd = v2 + d + v1 - "1524.5";          
         return jd;
}

BigNumber TimeInJulianCenturies(BigNumber jd){
	// (Meeus Pages 163-164) vvv
	//Time in Julian Centuries
        BigNumber t =((jd - "2451545.0")/"36525.0");
        return(t);
}

BigNumber MeanEquinoxOfTheDate(BigNumber t){
	BigNumber v1,v2,v4;
        //Mean equinox of the date
        v4 = "36000.76983";
        v1 = "0.0003032";
        v2 = "280.46645";
	BigNumber l = v2 + v4*t + v1*t*t;
        return l;
}

BigNumber MeanAnomalyOfTheSun(BigNumber t){
        BigNumber v1,v2,v3,v4;
        //Mean Anomaly of the Sun 
        v4 = "35999.05030";
        v1 = "0.0001559";
        v2 = "0.00000048";
        v3 = "357.52910";
        BigNumber m = v3 + v4 *t - v1 *t*t - v2*t*t*t; 
        return m;
}

BigNumber SunsEquationOfTheCenter(BigNumber m, BigNumber t){
        BigNumber v1,v2,v3,v4,v5,v6,v7,v8,v9;
	// Sun's Equation of the center
        v1 = "1.914600";
        v2 = "0.004817";
        v3 = "0.000014";
        v4 = BigNumberMath::sine(BigNumberMath::to_BigRad(m)); // sj
        v5 = "0.019993";
        v6 = "0.000101";
        v7 = BigNumberMath::sine(BigNumber(2)*BigNumberMath::to_BigRad(m)); // sj
        v8 = "0.000290";
        v9 = BigNumberMath::sine(BigNumber(3)*BigNumberMath::to_BigRad(m)); // sj
        BigNumber c = (v1 - v2 *t - v3 * t*t) * v4 + (v5 - v6*t)*v7 + v8*v9;
        return c;
}

BigNumber SunsApparantLongitude(BigNumber t, BigNumber o){
        BigNumber v1,v2,v3;
  	//Sun's Apparant Longitude (The Output of Lambda)
        v1="1934.136";
        v2="125.04";
	BigNumber omega = v2 - v1*t;
        v1="0.00569";
        v2="0.00478";
        v3= BigNumberMath::sine(BigNumberMath::to_BigRad(omega));   // sj
	BigNumber lambda = o - v1 - v2 * v3;
        return lambda;
}

 BigNumber ObliquityOfTheEcliptic(BigNumber t){
          BigNumber v1,v2,v3,v4,v5,v6;
          //Obliquity of the Ecliptic (Meeus page 147) (numbers switched from degree minute second in book to decimal degree)     
          v1="1934.136";
          v2="125.04";
	  BigNumber omega = v2 - v1*t;
          v1 = "23.4392966666667";
          v2 = "0.012777777777777778";
          v3 = "0.00059";
          v4 = "60.0";
          v5 = "0.00256";
          v6 = BigNumberMath::cosine(BigNumberMath::to_BigRad(omega));  // sj
          BigNumber epsilon = (v1 - v2*t - v3/v4 * t*t + v3/v4 * t*t*t) + (v5 * v6);
          return epsilon;
 }


 BigNumber SunsDeclination(BigNumber epsilon, BigNumber lambda){
          //Sun's Declination (Meeus page 165)  
          char buf1[15];
          BigNumber v1,v2,v3,v4;      
       if (calculationSpeed==1){
          v1 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(epsilon))),5,5,buf1);   // sj
          v2 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(lambda))),5,5,buf1);  // sj
       }
       if (calculationSpeed==0){
          v1 = BigNumberMath::sine(BigNumberMath::to_BigRad(epsilon));  // sj
          v2 = BigNumberMath::sine(BigNumberMath::to_BigRad(lambda));  // sj
       }
          v3 = v1 * v2;
        if (calculationSpeed==1){
          v4 = dtostrf(asin(bigNumberToFloat(v3)),5,5,buf1);
        }
       if (calculationSpeed==0){
        v4 = BigNumberMath::arcsine(v3);         // sj
        //v4 = dtostrf(asin(bigNumberToFloat(v3)),5,5,buf1);  // sj
       }

          BigNumber delta = BigNumberMath::to_BigDeg( v4 );  // sj
          return delta;
 }
 
 BigNumber SunsRightAscension(BigNumber epsilon, BigNumber lambda){
         BigNumber v1,v2,v3,v4;      
         char buf1[15];          
	//Sun's Right Acension (Meeus page 165) (divided by 15 to convert to hours)
       if (calculationSpeed==1){
        v1 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(epsilon))),5,5,buf1);  // sj
        v2 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(lambda))),5,5,buf1);  // sj
        v3 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(lambda))),5,5,buf1);  // sj
        v4 = dtostrf(atan2(bigNumberToFloat( (v1 * v2)) ,bigNumberToFloat(v3)),5,5,buf1);
       }
       if (calculationSpeed==0){
        v1 = BigNumberMath::cosine(BigNumberMath::to_BigRad(epsilon));  // sj
        v2 = BigNumberMath::sine(BigNumberMath::to_BigRad(lambda));  // sj
        
        //v3 = BigNumberMath::cosine(BigNumberMath::to_BigRad(lambda));  // sj
        // trick to calculate cosine
        if (region(BigNumberMath::to_BigRad(lambda))) v3 = BigNumberMath::minusOne*(BigNumberMath::one-v2*v2).sqrt();
        else v3 = (BigNumberMath::one-v2*v2).sqrt();
        
        //v4 = dtostrf(atan2(bigNumberToFloat( (v1 * v2)) ,bigNumberToFloat(v3)),5,5,buf1); // sj
        v4 = BigNumberMath::arctan2( v3 , (v1 * v2)); // sj - note: arguments reversed compared to atan2
       }
        BigNumber alpha = BigNumberMath::to_BigDeg(v4) / BigNumber(15);  // sj
        //long alpha2 = (long)alpha;
//	if (alpha2 < 0){
//	  alpha = alpha + BigNumber(24);
//        }
        if (alpha.isNegative() == 1){
	  alpha = alpha + BigNumber(24);
        }
        return alpha;
 }
BigNumber SiderealTime(BigNumber t, BigNumber jd){
        BigNumber v1,v2,v3,v4,v5,v6;
	//Sidereal Time (Meeus Page 88)
	v1 = "280.46061837";
        v2 = "360.98564736629";
        v3 = "2451545.0";
        v4 = "0.000387933";
        v5 = "38710000.0";
        v6 = "15.0";
        BigNumber theta = (v1 + v2 * (jd-v3) + v4*t*t - ((t*t*t)/v5))/v6;
        return theta;
}
BigNumber LocalHourAngle(BigNumber theta, BigNumber alpha, float longitude){
         char buf1[15];
         BigNumber v1,v3;
	 //The Local Hour Angle (Meeus Page 92) (multiplied by 15 to convert to degrees)
         v1 = dtostrf(longitude,5,5,buf1);
         v1 = (v1*BigNumber(-1))/BigNumber(15.0);
         v3 = "15.0";
	 BigNumber h = (theta - v1 - alpha)*v3;
         return h;
}
void findAlt(float latitude, BigNumber delta, BigNumber h){
        BigNumber v1,v2,v3,v4,v5,v6,v7,altitude2;
        char buf1[15];
        v6=dtostrf(latitude,5,5,buf1);
	//Local Horizontal Coordinates (Meeus Page 93)
      if (calculationSpeed==1){
        v1 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(v6))),5,5,buf1);  // sj
        v2 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(delta))),5,5,buf1);  // sj
        v3 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(v6))),5,5,buf1);  // sj
        v4 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(delta))),5,5,buf1);  // sj
        v5 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(h))),5,5,buf1);  // sj
      }
      if (calculationSpeed==0){
        v1 = BigNumberMath::sine(BigNumberMath::to_BigRad(v6));  // sj
        v2 = BigNumberMath::sine(BigNumberMath::to_BigRad(delta));  // sj
        
        //v3 = BigNumberMath::cosine(BigNumberMath::to_BigRad(v6));  // sj
        // trick to calculate cosine
        if (region(BigNumberMath::to_BigRad(v6))) v3 = BigNumberMath::minusOne*(BigNumberMath::one-v1*v1).sqrt();
        else v3 = (BigNumberMath::one-v1*v1).sqrt();
        
        //v4 = BigNumberMath::cosine(BigNumberMath::to_BigRad(delta));  // sj
        // trick to calculate cosine
        if (region(BigNumberMath::to_BigRad(delta))) v4 = BigNumberMath::minusOne*(BigNumberMath::one-v2*v2).sqrt();
        else v4 = (BigNumberMath::one-v2*v2).sqrt();
        
        v5 = BigNumberMath::cosine(BigNumberMath::to_BigRad(h));  // sj
      }
        v6 = (v1 * v2 + v3 * v4 * v5);
      if (calculationSpeed==1){
        v7 = dtostrf(asin(bigNumberToFloat(v6)),5,5,buf1);
      }
      if (calculationSpeed==0){
        //v7 = dtostrf(asin(bigNumberToFloat(v6)),5,5,buf1);
        v7 = BigNumberMath::arcsine(v6);  // sj
      }
	altitude2 = BigNumberMath::to_BigDeg(v7);  // sj
        SunsAltitude = bigNumberToFloat(altitude2);
}
void findAz(float latitude, BigNumber delta, BigNumber h){
        BigNumber v1,v2,v3,v6,v7;      
	//Local Horizontal Coordinates (Meeus Page 93)
        char buf1[15];
        v6=dtostrf(latitude,5,5,buf1);
       if (calculationSpeed==1){
        v1 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(h))),5,5,buf1);  // sj
       }
       if (calculationSpeed==0){
         v1 = BigNumberMath::sine(BigNumberMath::to_BigRad(h));  // sj
       }
        
        v7 = findAzSubFunction(v6, delta, h);
        
       if (calculationSpeed==1){
        v6 = dtostrf((atan2(bigNumberToFloat(v1),bigNumberToFloat(v7))),5,5,buf1);}        
       if (calculationSpeed==0){
        v6 = BigNumberMath::arctan2(v7,v1); // sj - note: arguments reversed compared to atan2
        //v6 = dtostrf((atan2(bigNumberToFloat(v1),bigNumberToFloat(v7))),5,5,buf1); // sj
       }

        v7 = BigNumberMath::to_BigDeg(v6);  // sj
	SunsAzimuth = bigNumberToFloat(v7);
}
BigNumber findAzSubFunction(BigNumber latitude, BigNumber delta, BigNumber h){
        BigNumber v2,v3,v4,v5,v6,v7,v8;
        char buf1[15];
      if (calculationSpeed==1){
        v2 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(h))),5,5,buf1);  // sj
        v3 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(latitude))),5,5,buf1);  // sj
        v7 = dtostrf(sin(bigNumberToFloat(BigNumberMath::to_BigRad(delta))),5,5,buf1);  // sj
        v8 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(delta))),5,5,buf1);  // sj
        v5 = dtostrf(cos(bigNumberToFloat(BigNumberMath::to_BigRad(latitude))),5,5,buf1);  // sj
      }
      if (calculationSpeed==0){
        v2 = BigNumberMath::cosine(BigNumberMath::to_BigRad(h));  // sj
        v3 = BigNumberMath::sine(BigNumberMath::to_BigRad(latitude));  // sj
        v7 = BigNumberMath::sine(BigNumberMath::to_BigRad(delta));  // sj
        
        //v8 = BigNumberMath::cosine(BigNumberMath::to_BigRad(delta));  // sj
        // trick to calculate cosine
        if (region(BigNumberMath::to_BigRad(delta))) v8 = BigNumberMath::minusOne*(BigNumberMath::one-v7*v7).sqrt();
        else v8 = (BigNumberMath::one-v7*v7).sqrt();
        
        //v5 = BigNumberMath::cosine(BigNumberMath::to_BigRad(latitude));  // sj
        // trick to calculate cosine
        if (region(BigNumberMath::to_BigRad(latitude))) v5 = BigNumberMath::minusOne*(BigNumberMath::one-v3*v3).sqrt();
        else v5 = (BigNumberMath::one-v3*v3).sqrt();
      }
        v4 =  v7/ v8;
        v7 = ((v2 * v3) - v4 * v5);
        return v7;
}

// true if z \elem [Pi/2,3*Pi/2] + n*Pi
boolean region(BigNumber z){
    BigNumber n;
    BigNumberMath::setScale(0);
    n = (z/BigNumberMath::piOverFour+BigNumberMath::two)/BigNumber(4);
    BigNumberMath::setScale(calculationScale);
    if ((int) n%BigNumberMath::two) return true;
    return false;
}

float bigNumberToFloat(BigNumber number){
  char buf1[15];
  BigNumber num1,num5;
  long num3, num4;
  num3 = (long)(number);
  num5 = dtostrf(num3,5,5,buf1);
  num1= (number - num5)*BigNumber(10000.0);
  num4 = (long)num1;
  return (float)num3+(((float)(num4))/10000.0);
}

// function to display a big number and free it afterwards
void printBignum (BigNumber & n)
{
  char * s = n.toString ();
  Serial.println (s);
  free (s);
}  // end of printBignum
