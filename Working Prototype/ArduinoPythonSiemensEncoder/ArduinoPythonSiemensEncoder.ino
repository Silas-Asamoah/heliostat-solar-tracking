/*
  DC motor position control system with encoder feedback.

  Arduino receives position to save, target position, emergency stop command via serial communication from Python interface,
  and responds actual position.
  Makes comparation between target and actual position and moves the drive to a target position.
  To control motor uses H-bridge.
  In this programm logic is invertet- to stop motor 1 is written.
  command sent to arduino using serial monitor.
  -----------------------------------------------------
  Serial protocol:
  send: 'your number'+
  it will save your number in myPosition1 variable
  send: 'you number'#
  it will save your number in myPosition2 variable
  send: <
  controller will respond number saved in myPosition2
  send: >
  controller will respond number saved in myPosition1
  send: 'your number'!
  it will save your number in position2reach variable
  send: (
  controller will respond number saved in position2reach
  send: p
  controller will respond actual position indicated by endocer
  send: s
  controller will receive Emergency Stop command.
  send: l
  controller will move slow left
  send r
  controller will move slow right
  ------------------------------------------------------------
*/

byte incomingByte ;
char byteToSendon;
char byteToSendoff;
long cyfra;
long number;
long myPosition1;
long myPosition2;
long position2reach;
long actualPos;
int oldPosition  = -999;
int motorLeftPin = 11;
int motorRightPin = 10;
int motorSpeed;
boolean EmergencyStop;
boolean moveStepLeftBit;
boolean moveStepRightBit;
#include <Encoder.h>
Encoder myEnc(2, 3);

void setup() {
  Serial.begin(115200);
  number = 0;
  EmergencyStop = 0;
  motorSpeed = 0;
  moveStepLeftBit = 0;
  moveStepRightBit= 0;
  
  pinMode(motorLeftPin, OUTPUT);
  pinMode(motorRightPin, OUTPUT);
  digitalWrite(motorLeftPin, 1);
  digitalWrite(motorRightPin, 1);
}
/*
  dystans-  1 turn (96 pulses) = 10mm (e.g.)
  1 puls = x one pulse -> const. = 0.1041 (10mm/96 pulses)
  SIEMENS encoder:
  5000 = 80mm
  1 puls = 0.016mm
*/
void loop() {
  actualPos = myEnc.read();

  //=============== Serial protocol ===========================
  while (Serial.available() > 0 ) {
    incomingByte = Serial.read();
    if (incomingByte > 47 && incomingByte < 58) {
      cyfra = incomingByte - 48;
      number = (number * 10) + cyfra;
    }
    if (incomingByte == 43) {    //sign + 43 write position 1
      myPosition1 = number;
      number = 0;
    }
    if (incomingByte == 35) {      //sign # 35 write position 2
      myPosition2 = number;
      number = 0;
    }
    if (incomingByte == 33) {      //sign ! 33 write target position
      position2reach = number;
      number = 0;
      EmergencyStop = 0;
    }

    if (incomingByte == 62) {     //sign >  62 asking about saved position 1
      byteToSendon = 63;            //sign ?
      Serial.print(byteToSendon);
      Serial.println(myPosition1);
      byteToSendon = 0;
      //Serial.flush();
    }
    if (incomingByte == 60) {     //sign <  60 asking about saved position 2
      byteToSendon = 38;            //sign &
      Serial.print(byteToSendon);
      Serial.println(myPosition2);
      byteToSendon = 0;
    }
    if (incomingByte == 40) {     //sign ( 40  asking about target position
      byteToSendon = 41;            //sign )
      Serial.print(byteToSendon);
      Serial.println(position2reach);
      byteToSendon = 0;
    }
    if (incomingByte == 115) {   //recive sign s go to emergency stop
      EmergencyStop = 1;
    }
   if (incomingByte == 112) {       // sign p  checking position
  byteToSendon = 97;             //sign a
  Serial.print(byteToSendon);
  Serial.println(actualPos);
  byteToSendon = 0;
}
 //------------------------------------------------------------------
    if (incomingByte == 108) {       //move slow left "l"
      moveStepLeftBit = 1;}
      else {
        moveStepLeftBit = 0;
      }
    
    if (incomingByte == 114) {       //move slow right "r"
      moveStepRightBit = 1;}
      else {
        moveStepRightBit = 0;
      } 
  
  //--------------------------------------------------------------------

  }
//================= End Serial protocol=========================
if (EmergencyStop == 0) {
  goToPosition();
}
  else {
    motorStop();
  }

if (moveStepLeftBit == 1) {
  moveStepLeft();
}
if (moveStepRightBit == 1) {
  moveStepRight();
}
  }
void moveStepLeft() {
  position2reach = actualPos - 1;
}
void moveStepRight() {
  position2reach = actualPos + 1;
}
void goToPosition() {
    if (actualPos > position2reach) {
      moveMotorLeft();
    }
    if (actualPos < position2reach) {
      moveMotorRight();
    }
    if (actualPos == position2reach) {
      //motorStop();
      digitalWrite(motorLeftPin, 1);
      digitalWrite(motorRightPin, 1);
    }
  }
  void moveMotorRight() {
    if ((position2reach - actualPos) < 10) { // reduce the speed if there is 10 point to the targer position
      motorSpeed = 200;
    } else {
      motorSpeed = 50;
    }
    analogWrite(motorRightPin, motorSpeed);
  }
  void moveMotorLeft() {
    if ((actualPos - position2reach) < 10) {
      motorSpeed = 200;
    } else {
      motorSpeed = 50;
    }
    analogWrite(motorLeftPin, motorSpeed);
  }
  void motorStop() {
    digitalWrite(motorLeftPin, 1);
    digitalWrite(motorRightPin, 1);
  }
