/*
  RTC_Code.h - Library for calculating the position of the sun
  Code by Maurice Ribble. Moved to Library by Gabriel P. Miller, July 22, 2012.

*/

#ifndef RTC_Code_h
#define RTC_Code_h
#include "Wire.h"  
  
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class RTC_Code
{
  public:
  static void CalculateSunsPositionLowAc(int month, int day, float hour, float minute, float second, float timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth);  
  
  static byte decToBcd(byte val);
  static byte bcdToDec(byte val);
  static void setDateDs1307(byte second,        // 0-59
                   byte minute,        // 0-59
                   byte hour,          // 1-23
                   byte dayOfWeek,     // 1-7
                   byte dayOfMonth,    // 1-28/29/30/31
                   byte month,         // 1-12
                   byte year);         // 0-99
                   
  static void getDateDs1307(byte *second,
          byte *minute,
          byte *hour,
          byte *dayOfWeek,
          byte *dayOfMonth,
          byte *month,
          byte *year);
          
  static void printtime(int hour, int minute, int second, int month, int dayOfMonth, int year, int dayOfWeek);
                   
  
};

#endif
