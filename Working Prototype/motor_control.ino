//connect motor controller pins to Arduino pins


//motor one
int enA = 10; 
int in1 = 9;
int in2 = 8;

//motor two
int enB = 5;
int in3 = 6;
int in4 = 7;

void setup()
{
    //setting all motor control pins to outputs
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

}

void demoOne()
{
    // Function that will run motor in both directions at a fixed speed
    // turn on Motor A 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    //set speed to 200 out of possible range 0 - 255
    analogWrite(enA);

    //turn on motor B 
    analogWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    // set speed to 200 out of possible range 0 - 255
    analogWrite(enB, 200);
    delay(2000);

    // Now Turn Off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW); 
}

void demoTwo()
{
    // This function will run the motors across the range of possible speeds
    // turn on motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    // accelerate from zero to maximum speed
    for (int i = 0; i < 256; i++)
    {
        analogWrite(enA, i);
        analogWrite(enB, i);
        delay(20);
    }

    // now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void loop()
{
    demoOne();
    delay(1000);
    demoTwo();
    delay(1000);
}