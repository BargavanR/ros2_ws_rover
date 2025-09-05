#include <Arduino.h>

#include "CytronMotorDriver.h"

// Create Cytron Objects (PWM_DIR, PWM, DIR)
CytronMD motor1(PWM_DIR, 3, 2);
CytronMD motor2(PWM_DIR, 5, 4);
CytronMD motor3(PWM_DIR, 6, 7);
CytronMD motor4(PWM_DIR, 9, 8);

// ====== Motor Control Functions ======
void forward(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}

void backward(int speed) {
  motor1.setSpeed(-speed);
  motor2.setSpeed(-speed);
  motor3.setSpeed(-speed);
  motor4.setSpeed(speed);
}

void left(int speed) {
  motor1.setSpeed(-speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(-speed);
}

void right(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(-speed);
  motor3.setSpeed(-speed);
  motor4.setSpeed(speed);
}

void spinLeft(int speed) {
  motor1.setSpeed(-speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(-speed);
  motor4.setSpeed(speed);
}

void spinRight(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(-speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(-speed);
}

void stopMotors() {
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  motor3.setSpeed(0);
  motor4.setSpeed(0);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Motor Control with Variable Speed Ready!");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); // read until newline

    if (cmd.length() > 1) {
      char action = cmd.charAt(0);       // f/b/l/r/e/q/s
      int speed = cmd.substring(1).toInt(); // extract number

      speed = constrain(speed, 0, 255);  // limit speed to motor driver range

      switch (action) {
        case 'f': forward(speed); break;
        case 'b': backward(speed); break;
        case 'l': left(speed); break;
        case 'r': right(speed); break;
        case 'e': spinLeft(speed); break;
        case 'q': spinRight(speed); break;
        case 's': stopMotors(); break;
      }
      Serial.print("Received: "); Serial.println(cmd);
    }
  }
}
