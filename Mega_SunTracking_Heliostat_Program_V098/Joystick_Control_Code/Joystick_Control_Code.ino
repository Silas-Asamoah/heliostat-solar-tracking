void checkJoystick(){
  if(digitalRead(plusOneButton) == HIGH && digitalRead(minusOneButton) == HIGH && joystickModeOnOff == 0 && joystickTriggerOnce == 0){
    joystickModeOnOff=1;
    joystickTriggerOnce=1;
    justFinishedManualControl=1;
    UDCenter = analogRead(joystickUDAnalogPin);
    LRCenter = analogRead(joystickLRAnalogPin);
    MachineOn(machineNumber);  
  }
  if(digitalRead(plusOneButton) == LOW && digitalRead(minusOneButton) == LOW){joystickTriggerOnce=0;}
  if(digitalRead(plusOneButton) == HIGH && digitalRead(minusOneButton) == HIGH && joystickModeOnOff == 1 && joystickTriggerOnce == 0){
    joystickModeOnOff=0;
    joystickTriggerOnce=1;
  }
  if(digitalRead(plusOneButton) == HIGH){
   manualMachineNumber=manualMachineNumber+1;
   if(manualMachineNumber>numberOfMachines-1){manualMachineNumber=0;}
   MachineOn(manualMachineNumber);  
   delay(200);
  }
  if(digitalRead(minusOneButton) == HIGH){
   manualMachineNumber=manualMachineNumber-1;
   if(manualMachineNumber<0){manualMachineNumber=numberOfMachines-1;}
   MachineOn(manualMachineNumber);  
   delay(200);
  }
  if(joystickModeOnOff==1){machineNumber=manualMachineNumber;}
  if(joystickModeOnOff==1){
  ManualControlThroughJoystick();
  }
}


void ManualControlThroughJoystick(){
  float UD = analogRead(joystickUDAnalogPin);
  float LR = analogRead(joystickLRAnalogPin);
  if (UD>UDCenter+15||UD<UDCenter-15||LR>LRCenter+15||LR<LRCenter-15){
    
    altMove = ((UD - UDCenter)/UDCenter)*0.75*invertUD;
    azMove = ((LR - LRCenter)/LRCenter)*0.75*invertLR;

    altManualSpeed = abs(altManualSpeedSwap*altMove); 
    azManualSpeed = abs(azManualSpeedSwap*azMove);

    if(altManualSpeed<5){altManualSpeed=5;}
    if(azManualSpeed<5){azManualSpeed=5;}
  
    if (digitalRead(HeliostatToSun)==HIGH || float(pgm_read_float(&MachineSettings[machineNumber][1]))==1){
     SunsAltitude=MachinesPrevAlt[machineNumber]+altMove;
     SunsAzimuth=MachinesPrevAz[machineNumber]+azMove;
    }
    
    if (float(pgm_read_float(&MachineSettings[machineNumber][1]))==2){
     MachineTargetAlt[machineNumber]=MachineTargetAlt[machineNumber]+altMove;
     MachineTargetAz[machineNumber]=MachineTargetAz[machineNumber]+azMove;     
    }
  }
   if (digitalRead(manualModeOnOffPin)==HIGH){
     MachineTargetAlt[machineNumber]=0;
     MachineTargetAz[machineNumber]=0;
   }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void joystickMoveMotors( long altsteps, int altitudeStepPin, int altitudeDirPin, float altManualSpeed, long azsteps, int azimuthStepPin, int azimuthDirPin, float azManualSpeed  ){
  float altMotorDelay, azMotorDelay, MotorDelay;
  long mostSteps;
  if (abs(altsteps)==altsteps){digitalWrite(altitudeDirPin, HIGH);}else{digitalWrite(altitudeDirPin, LOW);}  
  if (abs(azsteps)==azsteps){digitalWrite(azimuthDirPin, HIGH);}else{digitalWrite(azimuthDirPin, LOW);}  
  altMotorDelay = ( 1000000 * ( 60 / (steps * altManualSpeed) ) ) / 2;           
  azMotorDelay = ( 1000000 * ( 60 / (steps * azManualSpeed) ) ) / 2;           
  if (altMotorDelay<azMotorDelay){MotorDelay = altMotorDelay;}else{MotorDelay = azMotorDelay;}
  if(abs(altsteps)>abs(azsteps)){mostSteps=abs(altsteps);}else{mostSteps=abs(azsteps);}
  long x=0;
  while (x<mostSteps){
  if(x<abs(altsteps)){digitalWrite(altitudeStepPin, HIGH);}
  if(x<abs(azsteps)){digitalWrite(azimuthStepPin, HIGH);}
  delayInMicroseconds(MotorDelay);  
  if(x<abs(altsteps)){digitalWrite(altitudeStepPin, LOW);}
  if(x<abs(azsteps)){digitalWrite(azimuthStepPin, LOW);}
  delayInMicroseconds(MotorDelay);
  x+=1;
  }
}
