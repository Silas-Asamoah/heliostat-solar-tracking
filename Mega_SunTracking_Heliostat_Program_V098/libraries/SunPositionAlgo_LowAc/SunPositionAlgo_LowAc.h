/*
  SunPositionAlgo_LowAc.h - Library for calculating the position of the sun
  Created by Gabriel P. Miller, July 22, 2012.
  Attribution-NonCommercial-ShareAlike 3.0 United States (CC BY-NC-SA 3.0) (http://creativecommons.org/licenses/by-nc-sa/3.0/us/)
*/

#ifndef SunPositionAlgo_LowAc_h
#define SunPositionAlgo_LowAc_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SunPositionAlgo_LowAc
{
  public:
  static void CalculateSunsPositionLowAc(int month, int day, float hour, float minute, float second, float timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth, float &delta, float &h);  
};

#endif

