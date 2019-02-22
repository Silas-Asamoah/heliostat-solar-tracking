///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code moves the stepper motors
void moveToPosition(int AccelYesOrNo, long altsteps, long azsteps){

//    Serial.println("azsteps");
//    Serial.println(azsteps);  
//    Serial.println("altsteps");
//    Serial.println(altsteps);


  if (joystickModeOnOff==1 && iterationsAfterReset>0){//This code runs when manually controlling the machines through joystick
  joystickMoveMotors( altsteps, altitudeStepPin, altitudeDirPin, altManualSpeed, azsteps, azimuthStepPin, azimuthDirPin, azManualSpeed  );
  //moveMotorWithoutAccel(azsteps, azimuthStepPin, azimuthDirPin, azManualSpeed);
  //moveMotorWithoutAccel(altsteps, altitudeStepPin, altitudeDirPin, altManualSpeed);
  AccelYesOrNo = 3;
  }

  if (AccelYesOrNo==1){//This code runs during normal operation
  moveMotorWithAccel(azsteps, azimuthStepPin, azimuthDirPin, azSpeed, azAccel);
  moveMotorWithAccel(altsteps, altitudeStepPin, altitudeDirPin, altSpeed, altAccel);
  }
   
  }  


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code moves the stepper motors with acceleration
void moveMotorWithAccel(long numOfSteps,int stepPin, int DirPin, float maxspeed,float Accel){
     if (abs(numOfSteps)==numOfSteps){digitalWrite(DirPin, HIGH);}else{digitalWrite(DirPin, LOW);}  
     float minMotorDelay  = (1000000/((maxspeed*steps)/60))/2;
     float halfWay = abs(long(numOfSteps/2));
     float maxMotorDelay=((1000000/((maxspeed*steps)/60))*100)/2;
     float motorDelay=maxMotorDelay;  

     float motorDelayAdjust=((maxMotorDelay-minMotorDelay)*(maxMotorDelay-minMotorDelay))/(Accel*1000000);               
     float numberOfStepsToReachMinFromMax=long((maxMotorDelay-minMotorDelay)/motorDelayAdjust);    
     if(-0.00001<Accel&&Accel<0.00001){motorDelay=minMotorDelay;}
     
     if (abs(numOfSteps)>numberOfStepsToReachMinFromMax*2){//This code runs if the accleration slope "flat lines"
       for (long doSteps=1; doSteps <= abs(numOfSteps); doSteps++){
         delayInMicroseconds(motorDelay);
         digitalWrite(stepPin, HIGH);
         delayInMicroseconds(motorDelay);
         digitalWrite(stepPin, LOW);
          if ((motorDelay>minMotorDelay)&&(doSteps<halfWay)){motorDelay=motorDelay-motorDelayAdjust;}//Accelerates the motor
          if ((motorDelay<minMotorDelay)){motorDelay=minMotorDelay;}//Holds the motor at its max speed
          if ((doSteps>(abs(numOfSteps)-numberOfStepsToReachMinFromMax))){motorDelay=motorDelay+motorDelayAdjust;}//Deccelerates the motor after it gets close to the end of its move
        }
      }else{//This code runs if the acceleration slope is an upside down V.
        for (long doSteps=1; doSteps <= abs(numOfSteps); doSteps++){
          delayInMicroseconds(motorDelay);
          digitalWrite(stepPin, HIGH);     
          delayInMicroseconds(motorDelay);
          digitalWrite(stepPin, LOW);
          if ((motorDelay>minMotorDelay)&&(doSteps<halfWay)){motorDelay=motorDelay-motorDelayAdjust;}//Accelerates the motor
          if ((motorDelay<minMotorDelay)){motorDelay=minMotorDelay;}//Holds the motor at its max speed
          if (doSteps>halfWay){motorDelay=motorDelay+motorDelayAdjust;}//Deccelerates the motor after it gets close to the end of its move
          if (motorDelay>maxMotorDelay){motorDelay=maxMotorDelay;}   
        }
      } 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code is used to delay between stepper motor steps
void delayInMicroseconds(long delayInMicrosec){
     long t1,t2;
     t1=micros(); 
     t2=micros();
     while ((t1+delayInMicrosec)>t2){t2=micros();
     if (t2<t1){t2=t1+delayInMicrosec+1;}//Check if micros() rolled over
     }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code resets the machine on the limit switches
void findLimits(int altOrAz, int motorDirection, float limitAngle){
  if (altOrAz==1){ 
    searchForLimit(limitAngle, altitudeDirPin, altitudeStepPin, altResetSpeed, altLimitPin, altitudeMax, motorDirection );
  }
  if (altOrAz==2){
    searchForLimit(limitAngle, azimuthDirPin, azimuthStepPin, azResetSpeed, azLimitPin, azimuthMax, motorDirection );
  }
}
void searchForLimit(float limitAngle, int DirPin, int stepPin, int ResetSpeed, int LimitPin, long maxResetSteps, int motorDirection){
  long x;
  int whichDir;
  if (motorDirection!=0){//A motorDirection of 0 will skip the reset  
  if (abs(limitAngle) != limitAngle){whichDir = 1;}else{whichDir = -1;}
  if (motorDirection*whichDir != 1){digitalWrite(DirPin, LOW);}else{digitalWrite(DirPin, HIGH);}
    float MotorDelay = ( 1000000 * ( 60 / (steps * ResetSpeed) ) ) / 2; 
    while(x<maxResetSteps){
     digitalWrite(stepPin, HIGH);
     delayInMicroseconds(MotorDelay);          
     digitalWrite(stepPin, LOW); 
     delayInMicroseconds(MotorDelay);
     if (digitalRead(LimitPin)==HIGH){x=maxResetSteps;}
     x+=1;
   }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the angle to move the machine to after the limit switch has been triggered.
float positionAfterReset(float limitAngle){
  float endAltAndAz;
  if (abs(limitAngle) == limitAngle){endAltAndAz = limitAngle - moveAwayFromLimit;}else{endAltAndAz = limitAngle + moveAwayFromLimit;} 
  return endAltAndAz; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the angles for the heliostat (returnaltaz = 1 will return alt, 2 returns az)
void FindHeliostatAltAndAz(float SunsAltitude, float SunsAzimuth, float targetalt, float targetaz, float &machinealt, float &machineaz){

  float x,y,z,z1,z2,x1,x2,y1,y2,hyp,dist;
  
  z1 = sin(to_rad(SunsAltitude));
  hyp = cos(to_rad(SunsAltitude));
  x1 = hyp*cos(to_rad(SunsAzimuth*-1));
  y1 = hyp*sin(to_rad(SunsAzimuth*-1));

  z2 = sin(to_rad(targetalt));
  hyp = cos(to_rad(targetalt));
  x2 = hyp*cos(to_rad(targetaz*-1));
  y2 = hyp*sin(to_rad(targetaz*-1));  
  
  x=(x1-x2)/2+x2;
  y=(y1-y2)/2+y2;
  z=(z1-z2)/2+z2;
  
  dist=sqrt(x*x+y*y+z*z);
  if ((dist>-0.0001) && (dist <0.0001)){
  dist=0.0001;
  }

  machinealt=to_deg(asin(z/dist));
  machineaz=to_deg(atan2(y*-1,x));

}

float to_rad(float angle){
return angle*(pi/180);
}
float to_deg(float angle){
return angle*(180/pi);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This function returns a float number typed into the serial monitor
  float getFloatFromSerialMonitor(){
  //char inData[20];
  float f=0;    
  int x=0;  
  while (x<1){  
  String str;   
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    int i=0;
    // read all the available characters
    while (Serial.available() > 0) {
     char  inByte = Serial.read();
      str=str+inByte;
      inData[i]=inByte;
      i+=1;
      x=2;
    }
    f = atof(inData);
    memset(inData, 0, sizeof(inData));  
  }
  if (digitalRead(manualModeOnOffPin)==LOW){x=2;}
  }//END WHILE X<1  
   return f; 
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ManualControlThroughSerial(){
  float newAlt,newAz, newTargetAlt, newTargetAz;
    
  //if ((digitalRead(PinToMoveAltInPositiveDir)!=HIGH)&&(digitalRead(PinToMoveAltInNegativeDir)!=HIGH)
   //&&(digitalRead(PinToMoveAzInPositiveDir)!=HIGH)&&(digitalRead(PinToMoveAzInNegativeDir)!=HIGH)){
   int machineNumberInput;
   if (numberOfMachines>1){
   Serial.println("Manual Mode Active:");
   Serial.println("Type Machine Number:");   
   machineNumberInput = (int)getFloatFromSerialMonitor();  
   Serial.println(machineNumberInput);
   }else{machineNumberInput=0;}
   
   if (float(pgm_read_float(&MachineSettings[machineNumber][1]))==1 || digitalRead(HeliostatToSun)==HIGH){//MANUAL CONTROL OF SUN TRACKER
   Serial.println("Machine's Current Altitude:");   
   Serial.println(MachinesPrevAlt[machineNumberInput],4);      
   Serial.println("Machine's Current Azimuth:");      
   Serial.println(MachinesPrevAz[machineNumberInput],4);      
   Serial.println("Input Altitude:"); 
   newAlt = getFloatFromSerialMonitor();
   Serial.println(newAlt,4);   
   Serial.println("Input Azimuth:");    
   newAz = getFloatFromSerialMonitor();
   Serial.println(newAz,4);  

   Serial.println("Begin Move? Yes=1, No=0");
   int yesOrNo = getFloatFromSerialMonitor(); 
   Serial.println(yesOrNo); 
   if (yesOrNo==1){SunsAltitude=newAlt; SunsAzimuth=newAz;}
   }//END MANUAL CONTROL OF SUN TRACKER
   else{//Manual Control Of Heliostat Targets
   Serial.println("Target's Current Alt:");   
   Serial.println(MachineTargetAlt[machineNumberInput],4);      
   Serial.println("Target's Current Az:");      
   Serial.println(MachineTargetAz[machineNumberInput],4);      
   Serial.println("Input Target's New Alt:"); 
   newTargetAlt = getFloatFromSerialMonitor();
   Serial.println(newTargetAlt,4);   
   Serial.println("Input Target's New Az:");    
   newTargetAz = getFloatFromSerialMonitor();
   Serial.println(newTargetAz,4);   
   
   Serial.println("Begin Move? Yes=1, No=0");
   int yesOrNo = getFloatFromSerialMonitor(); 
   Serial.println(yesOrNo); 
   if (yesOrNo==1){MachineTargetAlt[machineNumberInput] = newTargetAlt; MachineTargetAz[machineNumberInput] = newTargetAz; targetsUsed = targetsUsed+1;}
   }//END MANUAL CONTROL OF HELIOSTAT TARGETS
   
   //} 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float eepromReadFloat(int address){
   union u_tag {
     byte b[4];
     float fval;
   } u;   
   u.b[0] = EEPROM.read(address);
   u.b[1] = EEPROM.read(address+1);
   u.b[2] = EEPROM.read(address+2);
   u.b[3] = EEPROM.read(address+3);
   return u.fval;
}
 
void eepromWriteFloat(int address, float value){
   union u_tag {
     byte b[4];
     float fval;
   } u;
   u.fval=value;
 
   EEPROM.write(address  , u.b[0]);
   EEPROM.write(address+1, u.b[1]);
   EEPROM.write(address+2, u.b[2]);
   EEPROM.write(address+3, u.b[3]);
}

//THESE FUNCTIONS WILL TURN THE PINS OF A MCP23017 ON AND OFF. THIS ALLOWS FOR THE CONTROL OF UP TO 16 MACHINES.
//EVEN MORE MACHINES COULD BE CONTROLLED BY ADDING MORE MCP23017s, BUT THIS CODE WOULD HAVE TO BE MODIFIED.
void turnMCP23017PinOn(int MachineNumber){
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x12); // GPIOA
    if(MachineNumber<8)
    {
    Wire.write((byte)machineToByte(MachineNumber));    // bank A
    Wire.write((byte)0);    // bank B
    }
    if(MachineNumber>=8){
    Wire.write((byte)0);    // bank A
    Wire.write((byte)machineToByte(MachineNumber-8));    // bank B
    }
    Wire.endTransmission();
    delay(100);
}
void turnMCP23017PinOff(){
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x12); // GPIOA
    Wire.write((byte)0);    // bank A
    Wire.write((byte)0);    // bank B
    Wire.endTransmission();
    delay(100);
}

byte machineToByte(int MachineNumber){
  byte x=2;
  if(MachineNumber==0){x=1;}
  if(MachineNumber>0){
  for (int i=1;i<MachineNumber;i++){
    x=2*x;
  }
  }
  return x;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code turns the machine on
void MachineOn(int number){
    turnMCP23017PinOn(number);
    if (enableHIGHorLOW==0){
    digitalWrite(EnablePin, LOW);
    }else{
    digitalWrite(EnablePin, HIGH);
    }
    
 }
 
//This code turns the machine off
void MachineOff(int number){
    turnMCP23017PinOff();
    if (digitalRead(manualModeOnOffPin)!=HIGH){
      if (enableHIGHorLOW==0){
      digitalWrite(EnablePin, HIGH);
      }else{
      digitalWrite(EnablePin, LOW);
      }
     } 
}
 
 
 
 
