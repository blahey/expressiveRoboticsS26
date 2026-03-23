// Stepper Configuration (Arduino Uno wiring)
// Motor: NEMA 17 Stepper Motor (200 steps/revolution at full stepping)
// Driver: A4988 Stepper Motor Driver
//
// Pin map aligned to the Uno-form-factor circuit layout:
// D2 -> DIR
// D3 -> STEP
// D4 -> MS1
// D5 -> MS2
// D6 -> MS3
// D7 -> ENABLE (active LOW)
//
// Power notes for A4988:
// - VDD to Arduino 5V
// - GND to Arduino GND (shared/common ground)
// - VMOT/GND to external motor supply

#include <AccelStepper.h>

const int dirPin = 2;
const int stepPin = 3;
const int ms1Pin = 4;
const int ms2Pin = 5;
const int ms3Pin = 6;
const int enablePin = 7;

AccelStepper myStepper(AccelStepper::DRIVER, stepPin, dirPin);

const int STEPS_PER_REV_FULL = 200;  // NEMA 17 motor: 200 steps/revolution at full stepping
const int MICROSTEP_MODE = 8;        // 1, 2, 4, 8, or 16

// Microstepping pin settings by MICROSTEP_MODE (MS1, MS2, MS3):
// 1  (full step):       LOW,  LOW,  LOW
// 2  (half step):       HIGH, LOW,  LOW
// 4  (quarter step):    LOW,  HIGH, LOW
// 8  (eighth step):     HIGH, HIGH, LOW
// 16 (sixteenth step):  HIGH, HIGH, HIGH
const int MS1_MODE = HIGH;
const int MS2_MODE = HIGH;
const int MS3_MODE = LOW;

int steps = 0;

// Uno only has A0-A5; use A0 if you enable variableStepper().
const int potPin = A0;

// Alternative to parseInt
// From: https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
// Example 4 - Receive a number as text and convert it to an int
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int dataNumber = 0;

void setup() {
  Serial.begin(9600);

  myStepper.setMaxSpeed(4000);     // steps per second
  myStepper.setAcceleration(500);  // steps per second^2
  myStepper.setSpeed(100);

  pinMode(ms1Pin, OUTPUT);
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(ms1Pin, MS1_MODE);
  digitalWrite(ms2Pin, MS2_MODE);
  digitalWrite(ms3Pin, MS3_MODE);
  digitalWrite(enablePin, LOW);  // LOW enables A4988 outputs
}

void loop() {
  randStepper();
  // variableStepper();
  // angleInputNB();
  // stepsInputNB();
}

void randStepper() {
  if (myStepper.distanceToGo() == 0) {
    // Random target position, speed, and acceleration.
    myStepper.moveTo(rand() % 8000);
    myStepper.setMaxSpeed((rand() % 2000) + 50);
    myStepper.setAcceleration((rand() % 1000) + 1);
  }
  myStepper.run();
}

void variableStepper() {
  int potVal = analogRead(potPin);
  float speed = map(potVal, 1023, 10, -4000, 4000);
  speed = constrain(speed, -4000, 4000);
  myStepper.setSpeed(speed);
  myStepper.runSpeed();
}

void angleInputNB() {  // non-blocking angle input
  recvWithEndMarker();

  if (newData) {
    int angle = dataNumber;
    Serial.print("angle = ");
    Serial.println(angle);

    int stepsPerRevolution = STEPS_PER_REV_FULL * MICROSTEP_MODE;
    angle = map(angle, 0, 360, 0, stepsPerRevolution);
    myStepper.moveTo(angle);
    newData = false;
  }
  myStepper.run();
}

void stepsInputNB() {  // non-blocking raw step input
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
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
      dataNumber = atoi(receivedChars);
    }
  }
}
