void moveMachine(float preTargetAlt, float preTargetAz, float targetalt, float targetaz, int sunTrackerOrHelio, 
float altGearRatio, float altMotorDirection, float altb, float altc, float altAngleAtZero, int altAcuteObtuse, float altLimitAngle, 
float azGearRatio,  float azMotorDirection,  float azb,  float azc,  float azAngleAtZero,  int azAcuteObtuse, float azLimitAngle, 
float minAz, float minAlt, float maxAz, float maxAlt){
        
   if (digitalRead(HeliostatToSun)==HIGH){sunTrackerOrHelio=1;}
   
   if ((iterationsAfterReset==0) || (FirstIterationAfterArduinoReset==0)){//Machine Resets Position
     if((digitalRead(WindProtectionSwitch)!= HIGH)){
     Serial.println("Resetting");
     MachineOn(machineNumber);  
     resetPositionOfMachine(altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero,
                                azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);
     }
   }

   if (iterationsAfterReset == 1){//Machine has just finished resetting position
     MachinesPrevAlt[machineNumber] = positionAfterReset(altLimitAngle);
     MachinesPrevAz[machineNumber] = positionAfterReset(azLimitAngle);
   } 
   
    float MachinesNewAltitude, MachinesNewAzimuth, altsteps, azsteps;
    float MachinesPreviousAltitude=MachinesPrevAlt[machineNumber];
    float MachinesPreviousAzimuth=MachinesPrevAz[machineNumber];

   if (SunsAltitude>=0 || digitalRead(WindProtectionSwitch)==HIGH || digitalRead(manualModeOnOffPin)==HIGH){//Waits until the sun is above the horizon before moving normally.
                                                                                                            //This code will also run if either manual mode or wind protection mode is activated.
    if (sunTrackerOrHelio == 1){//Machine Acts as Dual Axis Sun Tracker 
       MachinesNewAltitude = SunsAltitude;
       MachinesNewAzimuth = SunsAzimuth;
    }

    if (sunTrackerOrHelio == 2){//Machine Acts as Heliostat
       FindHeliostatAltAndAz(SunsAltitude, SunsAzimuth, targetalt, targetaz, MachinesNewAltitude, MachinesNewAzimuth); 
       if (joystickModeOnOff!=1){
         if(FirstIterationAfterArduinoReset!=0){
           
          Serial.print("Target's Alt: ");
          Serial.println(MachineTargetAlt[machineNumber],3);
          delay(50);
          Serial.print("Target's Az: ");
          Serial.println(MachineTargetAz[machineNumber],3);
          delay(50);

         }
       }
    }

    if (digitalRead(WindProtectionSwitch)==HIGH){
      MachinesNewAltitude = machineAltParkAngle;
      MachinesNewAzimuth = machineAzParkAngle;
    }

    if (joystickModeOnOff!=1){
         Serial.print("Machine's Alt: ");
         Serial.println(MachinesNewAltitude,3);
         delay(50);
         Serial.print("Machine's Az: ");
         Serial.println(MachinesNewAzimuth,3);
         delay(50);
    }     

//MACHINE ONLY MOVES WHEN REQUIRED POSITION IS WITHIN MACHINE'S LIMITS    
if ((MachinesNewAzimuth > minAz) && (MachinesNewAltitude > minAlt) && (MachinesNewAzimuth < maxAz) && (MachinesNewAltitude < maxAlt)){
if (iterationsAfterReset > 0){//This code does not run directly after machine resets.

//Machine uses Gear Reduction System for both AZIMUTH and ALTITUDE
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 0){
       azsteps = GearRatioMoveMotor(2, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection);   
       altsteps = GearRatioMoveMotor(1, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection);
    }
                          
//Machine uses Linear Actuator for both AZIMUTH and ALTITUDE   
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 1){
       azsteps = linearActuatorMoveMotor(2, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection, azb, azc, azAcuteObtuse, azAngleAtZero);
       altsteps = linearActuatorMoveMotor(1, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection, altb, altc, altAcuteObtuse, altAngleAtZero);
    }
                              
//Machine uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
   if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 2){
      azsteps = GearRatioMoveMotor(2, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection);   
      altsteps = linearActuatorMoveMotor(1, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection, altb, altc, altAcuteObtuse, altAngleAtZero);
   }
                              
//Machine uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
   if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 3){
      azsteps = linearActuatorMoveMotor(2, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection, azb, azc, azAcuteObtuse, azAngleAtZero);
      altsteps = GearRatioMoveMotor(1, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection);
   }

      if (abs(altsteps)>0 || abs(azsteps)>0){
        if(joystickModeOnOff!=1){
          MachineOn(machineNumber);  
        }
        
         moveToPosition(1,  altsteps, azsteps);

       }
       
       
    MachinesPrevAlt[machineNumber] = MachinesNewAltitude;
    MachinesPrevAz[machineNumber] = MachinesNewAzimuth;
    
  }//if (iterationsAfterReset > 1)     
  }else{Serial.println("Move exceeds bounds");
  if(joystickModeOnOff==1){
    if (HeliostatToSun==HIGH || float(pgm_read_float(&MachineSettings[machineNumber][1]))==1){
     SunsAltitude=MachinesPrevAlt[machineNumber]+altMove;
     SunsAzimuth=MachinesPrevAz[machineNumber]+azMove;
    }
    
    if (float(pgm_read_float(&MachineSettings[machineNumber][1]))==2){
     MachineTargetAlt[machineNumber]=MachineTargetAlt[machineNumber]+altMove;
     MachineTargetAz[machineNumber]=MachineTargetAz[machineNumber]+azMove;
    } 
  }
  }//END MACHINE ONLY MOVES WHEN REQUIRED POSITION IS WITHIN MACHINE'S LIMITS
  }//END (SunsAltitude>=0 || digitalRead(WindProtectionSwitch)==HIGH || digitalRead(manualModeOnOffPin)==HIGH)
 if(joystickModeOnOff!=1){
  MachineOff(machineNumber);  
 }

   
}//END MOVE MACHINE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long GearRatioMoveMotor(int altOrAz, float MachinesPreviousAngle, float MachinesNewAngle, float GearRatio, float MotorDirection){   
        float NumberOfSteps;
          if (altOrAz==1){
              NumberOfSteps = -1*steps * ((MachinesNewAngle-MachinesPreviousAngle) * GearRatio)/360.0 * MotorDirection+altLeftoverSteps[machineNumber];
              if (abs(NumberOfSteps)==NumberOfSteps){altLeftoverSteps[machineNumber]=abs(float(NumberOfSteps-float(long(NumberOfSteps))));}else{
              altLeftoverSteps[machineNumber]=abs(float(NumberOfSteps-float(long(NumberOfSteps))))*-1;}
           }
          if (altOrAz==2){
              NumberOfSteps = -1*steps * ((MachinesNewAngle-MachinesPreviousAngle) * GearRatio)/360.0 * MotorDirection+azLeftoverSteps[machineNumber];
              if (abs(NumberOfSteps)==NumberOfSteps){azLeftoverSteps[machineNumber]=abs(float(NumberOfSteps-float(long(NumberOfSteps))));}else{
              azLeftoverSteps[machineNumber]=abs(float(NumberOfSteps-float(long(NumberOfSteps))))*-1;}
           }
         return NumberOfSteps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetPositionOfMachine(float altMotorDirection, float altLimitAngle, float altGearRatio, float altb, float altc, int altAcuteObtuse, float altAngleAtZero,
                            float azMotorDirection,  float azLimitAngle,  float azGearRatio,  float azb,  float azc,  int azAcuteObtuse, float azAngleAtZero){
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 0){//Machine uses Gear Reduction System for both AZIMUTH and ALTITUDE
    gearReductionReset(2, azMotorDirection, azLimitAngle, azGearRatio); 
    gearReductionReset(1, altMotorDirection, altLimitAngle, altGearRatio);  
    }
    
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 1){//Machine uses Linear Actuator for both AZIMUTH and ALTITUDE
    linearActuatorReset(2, azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);    
    linearActuatorReset(1, altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero);        
    }
    
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 2){//Machine uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
    gearReductionReset(2, azMotorDirection, azLimitAngle, azGearRatio);
    linearActuatorReset(1, altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero);            
    } 
    
    if (float(pgm_read_float(&MachineSettings[machineNumber][0])) == 3){//Machine uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
    linearActuatorReset(2, azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);
    gearReductionReset(1, altMotorDirection, altLimitAngle, altGearRatio);
    }
    MachinesPrevAlt[machineNumber] = positionAfterReset(altLimitAngle); 
    MachinesPrevAz[machineNumber] = positionAfterReset(azLimitAngle); 
    //Serial.println("Reset Finished");
}//END reset
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gearReductionReset(int altOrAz, float MotorDirection, float LimitAngle, float GearRatio){
    findLimits(altOrAz, MotorDirection, LimitAngle);//Seeks out limit switch
    float dif = positionAfterReset(LimitAngle) - LimitAngle;    
    float NumberOfSteps = -1*steps * (dif * GearRatio)/360.0 * MotorDirection;    
    if (altOrAz==2){moveToPosition(1,  0, (NumberOfSteps));}//Moves motor away from limit switch
    if (altOrAz==1){moveToPosition(1,  (NumberOfSteps), 0);}//Moves motor away from limit switch    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
