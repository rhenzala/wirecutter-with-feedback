#include <Stepper.h>

const int stepsPerRevolution = 200;

const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

Stepper myStepper(stepsPerRevolution, motorPin1, motorPin2, motorPin3, motorPin4);

void setup() {
  myStepper.setSpeed(60);
  Serial.begin(9600);
}

void loop() {
  while (!Serial.available()) {}
  int numSteps = Serial.parseInt();
  myStepper.step(numSteps);
  delay(1000);
}
