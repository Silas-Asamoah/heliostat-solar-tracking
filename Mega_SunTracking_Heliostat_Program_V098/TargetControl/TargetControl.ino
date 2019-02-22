
void TargetControl(int second, int minute, int hour, int day, int month, int year, int dayOfWeek){
  
if (digitalRead(manualModeOnOffPin)!=HIGH && FirstIterationAfterArduinoReset!=0 && joystickModeOnOff!=1){


  
  /////////////////////////////////////////////////////////////////////////////
  //PROGRAM THE TARGETS HERE
  //////////////////////////////////////////////////////////////////////////////
  
   //READS POTENTIOMTER
   int analogReading = int(float(analogRead(A2))/10.0)*10;
   delay(500);
   int analogReading2 = int(float(analogRead(A2))/10.0)*10;
   while(abs(analogReading-analogReading2)>35){
   delay(500);
   analogReading = analogRead(A2);
   delay(500);
   analogReading2 = analogRead(A2);
   }
   
   //CHANGES TARGET GROUP BASED ON POTENTIOMETER READING
   if (analogReading>0){targetsUsed = 1;}
   if (analogReading>200){targetsUsed = 2;}
   if (analogReading>400){targetsUsed = 3;}
   if (analogReading>600){targetsUsed = 4;}
   if (analogReading>800){targetsUsed = 5;} 
   Serial.print("Current Target Group: ");
   Serial.println(targetsUsed);

//  float x;
//  x = getFloatFromSerialMonitor();
//  if (x>0){
//  targetsUsed = (int)x;
//  }
  
  //Serial.println("asdf");
  //Serial.println(targetsUsed);
//  if ((int)x==1){
//           Serial.print("test: ");
//           Serial.println("22222");
//    delay(20);
//  }
  
   
  //SAVES TARGET SETTINGS WHEN MANUAL CONTROL THROUGH EITHER THE SERIAL MONITOR OR THE JOYSTICK IS TURNED OFF
  if (justFinishedManualControl==1&&digitalRead(manualModeOnOffPin)!=HIGH){
    Serial.println("Writing to EPPROM");
      for (int i=0; i <= numberOfMachines - 1; i++){
        eepromWriteFloat(i*8+2*16*targetsUsed-1, MachineTargetAlt[i]);  
        eepromWriteFloat(i*8+4+2*16*targetsUsed-1, MachineTargetAz[i]);  
      }
    justFinishedManualControl=0;
   }
  
  //READS TARGET SETTINGS
   for (int i=0; i <= numberOfMachines - 1; i++){
       MachineTargetAlt[i] = eepromReadFloat(i*8+2*16*targetsUsed-1);
       MachineTargetAz[i]  = eepromReadFloat(i*8+4+2*16*targetsUsed-1);
   }
   

  //MachineTargetAlt[0] = 0;
  //MachineTargetAz[0]  = 0;
  
  
//Serial.println("Targets");
//Serial.println(targetsUsed);


  /////////////////////////////////////////////////////////////////////////////
  //END PROGRAM THE TARGETS
  //////////////////////////////////////////////////////////////////////////////
  
  
}//ENDif (digitalRead(manualModeOnOffPin)!=HIGH && FirstIterationAfterArduinoReset!=0)


}

