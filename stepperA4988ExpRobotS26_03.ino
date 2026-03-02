
// Stepper Configuration

#include <AccelStepper.h>
AccelStepper myStepper(1, 15, 14);  // 1 sets the MotorInterfaceType to DRIVER; pin 15 = step; pin 14 = direction
//AccelStepper myStepper(4, 0, 2, 1, 3); // define motor pins (0, 2, 1, 3) and interface mode (4)

const int ms1Pin = 18;          // define pins for stepping mode
const int ms2Pin = 17;          // stepping modes change the step resolution of the motor
const int ms3Pin = 16;          // higher resolution comes at the expense of higher speeds and torque
const int enablePin = 19;

int steps = 0;
int currentSteps = 0;
int previousSteps = 0;
// End Stepper Configuration

const int potPin = A13;

// Alternative to parseInt
// From: https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
// Example 4 - Receive a number as text and convert it to an int

const byte numChars = 32;
char receivedChars[numChars];  // an array to store the received data

boolean newData = false;

int dataNumber = 0;  // new for this version
// end Alternative to parseInt


void setup() {
  Serial.begin(9600);  myStepper.setMaxSpeed(4000); // sets the maximum steps per second, which determines how fast the motor will turn
  myStepper.setAcceleration(500); // sets the acceleration rate in steps per second
  myStepper.setSpeed(100);
  
  pinMode(ms1Pin, OUTPUT);        // set step mode pins as outputs
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(ms1Pin, HIGH);     // full step (LOW, LOW, LOW)
  digitalWrite(ms2Pin, HIGH);      // runs smoother in half step mode (HIGH, LOW, LOW) but might need to increase current for required torque
  digitalWrite(ms3Pin, LOW);
  digitalWrite(enablePin, LOW);
}

void loop() {
  //randStepper();
  // variableStepper();
  // angleInputNB();
  stepsInputNB();
}

void randStepper()
{
    if (myStepper.distanceToGo() == 0)
    {
  // Random change to speed, position and acceleration
  // Make sure we dont get 0 speed or accelerations
  //delay(1000);
  myStepper.moveTo(rand() % 8000);
  myStepper.setMaxSpeed((rand() % 2000) + 50);
  myStepper.setAcceleration((rand() % 1000) + 1);
    }
    myStepper.run();
}


  void variableStepper(){
    int potVal = analogRead(potPin);
    float speed = map(potVal, 1023, 10, -4000, 4000);
    speed = constrain(speed, -4000, 4000);
    myStepper.setSpeed(speed);
    myStepper.runSpeed();

  }


void angleInputNB() {  //non-blocking version of angle input
  steps = recvWithEndMarker(); // I'm reusing the steps, currentSteps and previousSteps variables rather than duplicating their function with "angle" versions. 
  currentSteps = steps;
  if (currentSteps != previousSteps) {
    int angle = steps;
    Serial.print("angle = ");
    Serial.println(angle);
    angle = map(angle, 0, 360, 0, 2038);  // Set the output range to match the steps/revolution of the motor.
    myStepper.moveTo(angle);
    previousSteps = currentSteps;
  }
  myStepper.run();
}

void stepsInputNB() {
  recvWithEndMarker();
  
  if (newData) {
    steps = dataNumber;
    Serial.print("steps = ");
    Serial.println(steps);
    myStepper.moveTo(steps);
    newData = false;
  }
  myStepper.run();
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  if (Serial.available() > 0) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newData = true;
      dataNumber = atoi(receivedChars);
    }
  }
}

