// Add to repository

#include <Servo.h>
#include <elapsedMillis.h>

Servo cameraServo;  // create servo object to control a servo
const int cameraServoPin = 7;

elapsedMillis cameraMovementTimer;
int cameraMovementInterval = 60;

int cameraMaxPosition = 180;
int cameraMinPosition = 0;
int cameraPosition = (cameraMaxPosition + cameraMinPosition) / 2;

int cameraPositionChange = 1; // clockwise = Positve value; counterclockwise = Negative value


void setup() {
  Serial.begin(9600);
  cameraServo.attach(cameraServoPin);  // attaches the servo on pin 9 to the servo object

}

void loop() {
  sweepCamera();

}

void sweepCamera() {
  if(cameraMovementTimer >= cameraMovementInterval) {
    cameraMovementTimer = 0; // reset the timer
    if (cameraPosition >= cameraMaxPosition || cameraPosition <= cameraMinPosition) {
      cameraPositionChange = cameraPositionChange * -1;
    }
    cameraPosition = cameraPosition + cameraPositionChange;
    cameraServo.write(cameraPosition);
  }

}