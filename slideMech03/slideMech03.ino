/*
 * Author: Byron Lahey
 * This code serves as an example of an integration of a DotStar (APA 102) LED strip 
 * and a stepper motor (using an A4988 stepper motor driver board).
 * 
 * This code builds on standard example code for each of the independent components. 
 * 
 * V01: 9/7/2021 -initial draft
 * V02: 9/8/2021 - added comments, cleaned up unused code
 * V03: 9/8/2021 -added randStepper() function. This is a "non-blocking" function, 
 *                allowing the LED animation and stepper movements to operate at
 *                the same time.
 * V04: Date???  -changes not recorded
 * V05: 9/19/2022 - Added setup for 28BYJ-48 Stepper Motor with ULN2003 Driver
 *                - Reference for hardware: https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
 *                - Reference for SPI on Teensy: https://www.pjrc.com/teensy/td_libs_SPI.html
 */





// Stepper Configuration

#include <AccelStepper.h>
AccelStepper myStepper(1, 15, 14);  // 1 sets the MotorInterfaceType to DRIVER; pin 15 = step; pin 14 = direction
//AccelStepper myStepper(4, 0, 2, 1, 3); // define motor pins (0, 2, 1, 3) and interface mode (4)

const int ms1Pin = 18;  // define pins for stepping mode
const int ms2Pin = 17;  // stepping modes change the step resolution of the motor
const int ms3Pin = 16;  // higher resolution comes at the expense of higher speeds and torque
const int enablePin = 19;
// End Stepper Configuration


// Timing Configuration

elapsedMillis myTimer01;
unsigned int timesUp01 = 100;
signed int pos = 0;        // used to keep track of the location of an LED
signed int posChange = 1;  // used to shift the position of the LED
// End Timing Configuration

// Limit Switch Configuration

const int limitSwitchPin = 23;
bool limitSwitchState = 1;

void setup() {
  Serial.begin(115200);
  delay(500);
  //while(!Serial);
  Serial.println("serial ready");


  myStepper.setMaxSpeed(2000);      // sets the maximum steps per second, which determines how fast the motor will turn
  myStepper.setAcceleration(1000);  // sets the acceleration rate in steps per second
  myStepper.setSpeed(100);

  //pinMode(resetPin, OUTPUT);
  //pinMode(sleepPin, OUTPUT);
  pinMode(ms1Pin, OUTPUT);  // set step mode pins as outputs
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(limitSwitchPin, INPUT);


  digitalWrite(ms1Pin, HIGH);  // full step (LOW, LOW, LOW)
  digitalWrite(ms2Pin, HIGH);  // runs smoother in half step mode (HIGH, LOW, LOW) but might need to increase current for required torque
  digitalWrite(ms3Pin, LOW);
  //digitalWrite(resetPin, HIGH);
  //digitalWrite(sleepPin, HIGH);
  digitalWrite(enablePin, LOW);
  delay(100);
  limitSwitchState = digitalRead(limitSwitchPin);
  
  homeSlide();
}

void loop() {

  //basicScript();
  //randomMotion01();
  //randStepper();
  //angleInput();
  //stepsInput();
  motionCycle();
}

void homeSlide() {

  

  while (limitSwitchState == 1) {
    limitSwitchState = digitalRead(limitSwitchPin);
    Serial.print("limitSwitchState = ");
    Serial.print(limitSwitchState);
    Serial.print("\t");
    Serial.println("homing...");
    delay(10);
    myStepper.setMaxSpeed(1000);
    myStepper.setSpeed(200);
    myStepper.runSpeed();
  }
  
  
 myStepper.setCurrentPosition(0);
  myStepper.setSpeed(400);
  myStepper.moveTo(-400);
  myStepper.runToPosition();
  myStepper.setCurrentPosition(0);
  Serial.println("Homed");
  Serial.flush();
  
 
}

void stepsInput() {
  int steps;
  if (Serial.available() > 1) {
    steps = Serial.parseInt();
    Serial.print("stepInput = ");
    Serial.println(steps);
    if (Serial.read() == 10) {  // newline character
      myStepper.setMaxSpeed(11000);
      myStepper.setSpeed(10000);
      myStepper.moveTo(steps);
      myStepper.runToPosition();
    }
  }
}



void basicScript() {
  myStepper.moveTo(-12000);
  myStepper.runToPosition();
  myStepper.moveTo(0);
  myStepper.runToPosition();
}

void randomMotion01() {
  myStepper.moveTo(random(4000));
  myStepper.runToPosition();
}

void randStepper() {
  if (myStepper.distanceToGo() == 0) {
    // Random change to speed, position and acceleration
    // Make sure we dont get 0 speed or accelerations
    //delay(1000);
    myStepper.moveTo(rand() % 4000);
    myStepper.setMaxSpeed((rand() % 800) + 1);
    myStepper.setAcceleration((rand() % 500) + 1);
  }
  myStepper.run();
}

void motionCycle() {
  if (myStepper.distanceToGo() == 0) {
    // Random change to speed, position and acceleration
    // Make sure we dont get 0 speed or accelerations
    //delay(1000);
//    if(cyclePosition == -12000) {
//      cyclePostion = 0;
//    }
//    else {
//      cyclePosition = -1200;
//    }
//    myStepper.moveTo(-rand() % 8000);
//    myStepper.setMaxSpeed((rand() % 8000) + 1);
//    myStepper.setAcceleration((rand() % 5000) + 1);

    myStepper.moveTo(-rand() % 3000);
    myStepper.setMaxSpeed(1000);
    myStepper.setAcceleration(500);
  }
  myStepper.run();
}

void angleInput() {
  int angle;
  if (Serial.available() > 0) {
    angle = Serial.parseInt();
  }
  if (Serial.read() == 10) {             // newline character
    angle = map(angle, 0, 360, 0, 400);  // Set the output range to match the steps/revolution of the motor.
    myStepper.moveTo(angle);
    myStepper.runToPosition();
  }
  delay(1);
}
