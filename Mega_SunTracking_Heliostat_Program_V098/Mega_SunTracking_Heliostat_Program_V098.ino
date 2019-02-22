
//**************************************************************
//  Name    : Simple Arduino Sun Tracking / Heliostat Control Program  
//  Author  : Gabriel Miller      
//  Website : http://www.cerebralmeltdown.com                                   
//  Notes   : Code for controlling various types of solar       
//          : machines.                                         
//****************************************************************
  #include "Wire.h"  
  #include <Flash.h>
  #include <EEPROM.h>
  #include "RTC_Code.h"
  #include <BigNumber.h>
  #include <BigNumberMath.h>
  

//PUT YOUR LATITUDE, LONGITUDE, AND TIME ZONE HERE
  float latitude = 39.19;
  float longitude = -78.16;
  float timezone = -5;
  
//If you live in the northern hemisphere, put 0 here. If you live in the southern hemisphere put 1.
  int useNorthAsZero = 0;

//INTERVAL BETWEEN MACHINE POSITION UPDATES
//Lets the program know how often to update the position of the machine(s) (in seconds)
  unsigned long updateEvery = 30;//seconds

//DEGREES TO MOVE OFF OF LIMIT SWITCHES
//Tells the program how many degrees to move off of limit switches after they have been triggered.
//Choose a positive number.
 float moveAwayFromLimit = 5;


//TIME MACHINE(S) RESET 
//Put the hour you want your solar machines to reset on 
//their limit switches here. Remember that daylight saving time is not used, so the
//hour it resets won't necessarily match the clock in your home.
  int hourReset = 21;

//Machine Wind Protection Park Angle
 float machineAltParkAngle=65;
 float machineAzParkAngle=0;

  // STEPPER MOTOR SETUP 
  //The number of steps required for your stepper to make one revolution. (Don't forget to take into 
  //account the settings on your driver board. i.e. Microstepping, half stepping etc.)
  //float steps = 800;
  float steps = 192;
  //Set the travel speed for your stepper motors here (Roughly in Rev/Min)
  float altSpeed=150;
  float azSpeed=150; 
  //Set the speed the motors travel when they reset here (Roughly in Rev/Min)
  //Note: Max reset speed is limited since checking to see if the limit has been triggered slows things down. 
  //The result of which means that the alt/az reset speeds may seem noticably slower than normal travel speed.
  float altResetSpeed=150; 
  float azResetSpeed=150;
  //Speed of Stepper Motors when using manual control through joystick
  float altManualSpeed=150; 
  float azManualSpeed=150;

  //Number of seconds (roughly) to accelerate to full speed.
  float altAccel = 1;
  float azAccel = 1;  
  //the maximum number of steps the machine(s) will take when resetting
  long altitudeMax = 1000000;
  long azimuthMax  = 1000000;
  
  //enable on HIGH or LOW
  int enableHIGHorLOW = 0;//Put a 0 here if your driver boards enable the stepper motors when the enable pin is 
                          //written LOW. 
                          //Put a 1 here if your driver boards enable the stepper motors when the enable pin is 
                          //written HIGH.


  /////////////////////////////////////////////////////////// 
  //PIN ASSIGNMENT
  ///////////////////////////////////////////////////////////
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //TWO WIRE STEP/DIR DRIVER BOARD Pin Assignment
    int azimuthStepPin = 2;
    int azimuthDirPin = 3;  
    
    int altitudeStepPin = 4;
    int altitudeDirPin = 5;
    
    int EnablePin = 6;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Limit Switch Pin Assignment
    const int altLimitPin = 7;
    const int azLimitPin = 7; 
    
    //Manual Control Pin Assignment
    const int manualModeOnOffPin = 8;
    //Wind Protection Switch Pin Assignment
    const int WindProtectionSwitch = 10;
    //Heliostat to Sun Tracker Toggle Switch pin
    const int HeliostatToSun = 9;
    
    //Joystick Up Down and Left Right pins.
    int joystickUDAnalogPin = A0;
    int joystickLRAnalogPin = A1;
    int invertUD = -1;//Change value from 1 to -1 to invert up/down during joystick control
    int invertLR = -1;//Change value from 1 to -1 to invert left/right during joystick control

    //Select machine to manually control through joystick button pins
    int plusOneButton = 11;
    int minusOneButton = 12;

   
    //Put how many machines you want to control here
    #define numberOfMachines 1
    
    //Open the "ReadMe" file that was downloaded with this program to see what each of these settings do.
    const float MachineSettings[][20] PROGMEM = { 
    //             LorG    SorH   Al_GRorTPU   Al_Mdir  Al_L.A  Al_L.B  Al_S.A.  Al_AcAb  Al_LS  Az_GRorTPU  Az_Mdir  Az_L.A  Az_L.B  Az_S.A.  Az_AcAb  Az_LS   MinAz  MinAlt  MaxAz  MaxAlt
    //Setting #      0       1         2          3       4       5       6         7       8        9         10       11      12      13       14      15       16     17     18     19
                //Example Machine That uses Gear Reduction System for both AZIMUTH and ALTITUDE
                 //{   0  ,    2 ,       36 ,       1 ,     0  ,    0  ,    0  ,      0  ,   -1,     36 ,    1 ,      0 ,     0 ,     0 ,      0,    -110,   -110,   -1,   130,   110 }
                //,{   0  ,    2 ,       36 ,       1 ,     0  ,    0  ,    0  ,      0  ,   -1,     36 ,    1 ,      0 ,     0 ,     0 ,      0,    -110,   -110,   -1,   130,   110 }
               //Example Machine That uses Linear Actuator for both AZIMUTH and ALTITUDE   
                {    1  ,    2 ,       14 ,      1 ,    8.21 ,  8.875 , 78.41 ,     1   ,  -1.5 ,     14 ,      -1  ,     5.5 ,    5.5 ,  90 ,     2,  -58.839, -58.839, -1.5   ,  65,   66 }
                //Example Machine That uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
                  //,{ 2 ,       1,         2,       -1 ,    956,  1016,      90,       1,     -1,    5067,   -1,       0,      0,      0,       0,    -94, -94, -1,    120,    88}
                //Example Machine That uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
                //,{ 3  ,     1 ,        36 ,       1 ,     0 ,    0 ,       0 ,      0,     -1,     20 ,    1 ,      16,     11,     90 ,      1,   -60 , -60, -1,    60,   160 }
   
     };
      

    

    
    
    
    
    
    
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//END OF USER SETTINGS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    
    
    
//BigNumber Library Settings
  int calculationScale=10;
  int calculationSpeed=0;// 0 for slow, 1 for fast (but less accurate)
//
    
    
    


/////////////////////////////////////////////////////////// 
//MISC. VARIABLES USED THROUGHOUT THE PROGRAM
///////////////////////////////////////////////////////////  
  float pi = 3.14159265, SunsAltitude, SunsAzimuth, h, delta;
  //float helioaltdif, helioazdif, prehelioalt, prehelioaz, helioalt, helioaz;  
  //float preAlt = 0, preAz = 0, altdif, azdif;
  int iterationsAfterReset, preTargetsUsed, machineNumber, windToggleCount, midCycle, FirstIterationAfterArduinoReset, justFinishedManualControl;
  int  targetsUsed = 1;
  unsigned long updateTime = 0,  now = 0; 
  float altManualSpeedSwap, azManualSpeedSwap, altMove, azMove, UDCenter, LRCenter;
  int joystickModeOnOff, joystickTriggerOnce, manualMachineNumber;

/////////////////////////////////////////////////////////// 
//END MISC. VARIABLES
///////////////////////////////////////////////////////////
  
  
///////////////////////////////////////////////////////////  
//MISC. ARRAYS  
///////////////////////////////////////////////////////////  
  float PrevAltTargetsArray[numberOfMachines];
  float PrevAzTargetsArray[numberOfMachines];
  float altLeftoverSteps[numberOfMachines];
  float azLeftoverSteps[numberOfMachines];
  float MachineTargetAlt[numberOfMachines];
  float MachineTargetAz[numberOfMachines];
  float MachinesPrevAlt[numberOfMachines];
  float MachinesPrevAz[numberOfMachines];  
   
  //float MachineAlt[numberOfMachines];
  //float MachineAz[numberOfMachines];
  
  char inData[20];
///////////////////////////////////////////////////////////  
//END MISC. ARRAYS  
///////////////////////////////////////////////////////////  
  
  
  
  
  
  
  
  
  
/////////////////////////////////////////////////////////// 
// VOID SETUP
/////////////////////////////////////////////////////////// 
  void setup(){

  Wire.begin();
  Serial.begin(9600);
  
  
  ////////////////////////////////              
  //TWO WIRE STEP/DIR DRIVER BOARD CODE  
  pinMode(altitudeStepPin, OUTPUT);
  pinMode(altitudeDirPin, OUTPUT);
  pinMode(azimuthStepPin, OUTPUT);
  pinMode(azimuthDirPin, OUTPUT);
  pinMode(EnablePin, OUTPUT);  
  ////////////////////////////////  

//Arduino pins automatically default to inputs, so this code has thus been commented out.
//  pinMode(azLimitPin, INPUT);
//  pinMode(altLimitPin, INPUT);
//  pinMode(WindProtectionSwitch, INPUT);
//  pinMode(manualModeOnOffPin, INPUT);
//  pinMode(HeliostatToSun, INPUT);
//  pinMode(plusOneButton, INPUT);
//  pinMode(minusOneButton, INPUT);

  altManualSpeedSwap = altManualSpeed; 
  azManualSpeedSwap = azManualSpeed;
  
//MCP23017 SETUP CODE FROM http://tronixstuff.wordpress.com/tutorials > chapter 41
  // setup addressing style
  Wire.beginTransmission(0x20);
  Wire.write((byte)0x12);
  Wire.write((byte)0x20); // use table 1.4 addressing
  Wire.endTransmission();

  // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write((byte)0x00); // IODIRA register
  Wire.write((byte)0x00); // set all of bank A to outputs
  Wire.write((byte)0x00); // set all of bank B to outputs
  Wire.endTransmission();
//END MCP23017 SETUP CODE
 for (int i=0; i <= numberOfMachines - 1; i++){MachineOff(i);}//Makes sure machines are off

}
/////////////////////////////////////////////////////////// 
//END VOID SETUP
/////////////////////////////////////////////////////////// 






/////////////////////////////////////////////////////////// 
// VOID LOOP
/////////////////////////////////////////////////////////// 
void loop()
{  
  
  int month, year, day, dayOfWeek;
  float hour, minute, second;
  byte secondRTC, minuteRTC, hourRTC, dayOfWeekRTC, dayOfMonthRTC, monthRTC, yearRTC;
   
  RTC_Code::getDateDs1307(&secondRTC, &minuteRTC, &hourRTC, &dayOfWeekRTC, &dayOfMonthRTC, &monthRTC, &yearRTC);
  if (joystickModeOnOff!=1){
  Serial.println(" ");   
  Serial.print("Time: ");
  RTC_Code::printtime(hourRTC, minuteRTC, secondRTC, monthRTC, dayOfMonthRTC, yearRTC, dayOfWeekRTC);//Displays the RTC time
  delay(500);
  }
  
  //The variables below can be set to accept input from devices other than an RTC.
  dayOfWeek=dayOfWeekRTC;//NOT CURRENTLY USED
  year = yearRTC;//NOT CURRENTLY USED
  month = monthRTC;
  day = dayOfMonthRTC;
  hour = hourRTC;
  minute = minuteRTC;
  second = secondRTC; 
  /////////////////////////////////////////////////////////////////////////////////
  

  
  if ((digitalRead(manualModeOnOffPin)!=HIGH) && (joystickModeOnOff!=1)){    
   if ((hour == hourReset)&&(minute == 0)&&(second < 10)){//If the variable "iterationsAfterReset" is set to 0, the machines reset themselves on the limit switches
     iterationsAfterReset = 0; 
   } 
  now = millis();
  if ( (now + updateEvery*1000) < updateTime){updateTime=now+updateEvery;}
  }//END if ((digitalRead(manualModeOnOffPin)!=HIGH))
  
  
  
  
  //USE THIS CODE TO RUN THE PROGRAM AT FAST SPEED. DON"T FORGET TO UNCOMMENT THE } } TOWARD THE BOTTOM
  //YOU MAY ALSO WISH TO COMMENT OUT THE "MachineOn" AND "MachineOff" CODE TO GET RID OF THE DELAY
  //  month = 6;
  //  day = 9;
  //  for (float hour=8; hour <= 21; hour++){
  ////  Serial.print("Hour ");
  ////  Serial.println(hour);
  ////  Serial.println(TotalAltSteps);
  ////  Serial.println(TotalAzSteps);
  //    for (float minute=0; minute <= 59; minute++){
  //    secondCounter=updateEvery+1;      
  ////  Serial.print("Minute ");
  ////  Serial.println(minute);



  //if (digitalRead(WindProtectionSwitch)!=HIGH && (FirstIterationAfterArduinoReset!=0)){  
  //Updates the position of the sun, which in turn tells the machine(s) that it is time to move.
  if ((now>=updateTime)||(iterationsAfterReset == 0)){
       updateTime=updateEvery*1000 + millis();

      //SunPositionAlgo_LowAc::CalculateSunsPositionLowAc(month, day, hour, minute, second, timezone, latitude, longitude, SunsAltitude, SunsAzimuth, delta, h);
      year = year + 2000;
      findSunsAltAndAzOne(year, month, day, timezone, hour, minute, second, latitude, longitude);
      SunsAltitude = SunsAltitude + (1.02/tan((SunsAltitude + 10.3/(SunsAltitude + 5.11)) * pi/180.0))/60.0;//Refraction Compensation: Meeus Pg. 105

      if ((joystickModeOnOff!=1)){
           Serial.print("Sun's Alt: ");
           Serial.println(SunsAltitude,3);
           delay(50);
           Serial.print("Sun's Az: ");
           Serial.println(SunsAzimuth , 3);
           delay(50);
           Serial.print("Number of Machines ");
           Serial.println(numberOfMachines);
           delay(50);
      }
      if(useNorthAsZero==1){
      if (SunsAzimuth<0){
      SunsAzimuth=(SunsAzimuth+180)*-1;
      }
      if (SunsAzimuth>0){
      SunsAzimuth=(SunsAzimuth-180)*-1;
      }
      
      //Serial.print("Sun's Azimuth Modified for Southern Hemisphere: ");
      //Serial.println(SunsAzimuth);
      }

  }//END Update Every X seconds

    TargetControl(second, minute, hour, day, month, year, dayOfWeek);//Checks to see if the targets have been changed  
    checkJoystick();
  
  
  //This code allows for manual control of the machine through the 
  //Serial Monitor when the "manual switch" is turned on.
  if ((digitalRead(manualModeOnOffPin)==HIGH)&&FirstIterationAfterArduinoReset!=0){ 
     if(joystickModeOnOff!=1){//prevents manual control through both joystick and serial at the same time
       ManualControlThroughSerial();
       updateTime = millis();
       justFinishedManualControl=1;
     }
  }
  //END of manual control code

  for (int i=0; i <= numberOfMachines - 1; i++){

  if (joystickModeOnOff!=1){   
  machineNumber=i;//Put which machine you are trying to control here
  Serial.print("Machine Number ");
  Serial.println(machineNumber);
  delay(50);
   
  
  }


  moveMachine(PrevAltTargetsArray[machineNumber], PrevAzTargetsArray[machineNumber],
  MachineTargetAlt[machineNumber], MachineTargetAz[machineNumber],
         float(pgm_read_float(&MachineSettings[machineNumber][1])),     
         float(pgm_read_float(&MachineSettings[machineNumber][2])), 
         float(pgm_read_float(&MachineSettings[machineNumber][3])), 
         float(pgm_read_float(&MachineSettings[machineNumber][4])), 
         float(pgm_read_float(&MachineSettings[machineNumber][5])), 
         float(pgm_read_float(&MachineSettings[machineNumber][6])), 
         float(pgm_read_float(&MachineSettings[machineNumber][7])), 
         float(pgm_read_float(&MachineSettings[machineNumber][8])),
         float(pgm_read_float(&MachineSettings[machineNumber][9])),  
         float(pgm_read_float(&MachineSettings[machineNumber][10])),  
         float(pgm_read_float(&MachineSettings[machineNumber][11])), 
         float(pgm_read_float(&MachineSettings[machineNumber][12])),  
         float(pgm_read_float(&MachineSettings[machineNumber][13])),  
         float(pgm_read_float(&MachineSettings[machineNumber][14])),
         float(pgm_read_float(&MachineSettings[machineNumber][15])),
         float(pgm_read_float(&MachineSettings[machineNumber][16])),
         float(pgm_read_float(&MachineSettings[machineNumber][17])),
         float(pgm_read_float(&MachineSettings[machineNumber][18])),
         float(pgm_read_float(&MachineSettings[machineNumber][19]))         
    );

   PrevAltTargetsArray[machineNumber] = MachineTargetAlt[machineNumber];
   PrevAzTargetsArray[machineNumber] = MachineTargetAz[machineNumber];
   preTargetsUsed = targetsUsed;  
   
   if(joystickModeOnOff==1){break;}
  }//END for (float i=1; i <= numberOfMachines; i++)
     
  if (iterationsAfterReset < 3){iterationsAfterReset+=1;}
   
   if (digitalRead(WindProtectionSwitch)==HIGH && (midCycle==1)){
   int iloop=0;
   while (iloop<2){
   delay(1000); Serial.println("Wind Protection Mode Enabled"); iterationsAfterReset=0; midCycle=0;
   if (digitalRead(WindProtectionSwitch)==LOW){iloop=5;}
   }}
   if (digitalRead(WindProtectionSwitch)==HIGH){midCycle=midCycle+1;}   


  if (digitalRead(WindProtectionSwitch)==HIGH){delay(1000); //Serial.println("Wind Protection Mode Enabled"); 
  iterationsAfterReset=0;}  
  FirstIterationAfterArduinoReset=1;
//}//END do not reset after arduino resets when Wind Protection Switch is high
//  //}}//End for loops that run the program at fast speed

}//End Void Loop
/////////////////////////////////////////////////////////// 
// END VOID LOOP
/////////////////////////////////////////////////////////// 




