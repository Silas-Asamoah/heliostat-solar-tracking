/*
Program to control two servos (Pitch and Yaw) with a simple plain-text protocol via Serial
115200 baud

Pddd\n: Move Pitch to ddd degrees. E.g. "P000\n" moves it to 0 degrees, "P090\n" to 90 degrees
Yddd\n: Move Yaw to ddd degrees. E.g. "Y180\n" moves it to 180 degrees

   */

#include <Servo.h>

// Pins for the servos
#define SERVO_PITCH 12
#define SERVO_YAW 13

Servo servoPitch, servoYaw;
String msg;

void setup(){
	// Attach the servos
	servoPitch.attach(SERVO_PITCH);
	servoYaw.attach(SERVO_YAW);

	servoPitch.write(0);
	servoYaw.write(0);

	// Begin serial communication
	Serial.begin(115200);

	Serial.println("!");
}


void loop(){

	// Wait for 6 characters to arrive (Letter, four digits, and newline)
	if(Serial.available()>=6){
		// Read the characters
		msg = Serial.readStringUntil('\n');	

		// Convert the three digits to an integer number
		// Each of them is converted from a character to its corresponding numerical value when substracting the character '0'
		// Then they are multiplied to the value corresponding to its position (100, 10, 0)
		float deg = int(msg.charAt(1)-'0')*100 + int(msg.charAt(2)-'0')*10 + int(msg.charAt(3)-'0') + int(msg.charAt(4)-'0')*0.1;

		// Echo the msg
		Serial.println(msg);

		// Print the calculated degrees number
		Serial.println(deg);

		// Use the first character to decide which servo to move, Pitch or Yaw
		// and move it by the given amount of degrees
		switch(msg.charAt(0)){
			case 'P':
				servoPitch.write(deg);
			break;
			case 'Y':
				servoYaw.write(deg);
			break;

		}

	}

}
