#include "A4988.h"

int Step = 3; // GPIO3 in Arduino UNO --- Step of stepper motor driver
int Dire  = 2; // GPIO2 in Arduino UNO --- Direction of stepper motor driver
int Sleep = 4; // GPIO4 in Arduino UNO --- Control Sleep Mode on A4988
int MS1 = 7; // GPIO7 in Arduino UNO --- MS1 for A4988
int MS2 = 6; // GPIO6 in Arduino UNO --- MS2 for A4988
int MS3 = 5; // GPIO5 in Arduino UNO --- MS3 for A4988

// Motor Specs
const int spr = 200; // Steps per revolution
int RPM = 100; // Motor Speed in revolutions per minute
int Microsteps = 1; // Step size (1 for full steps, 2 for half steps, etc.)
bool isRotating = false; // To track if the motor is rotating
String direction = "cw"; // Default direction
//int directionValue = 0;

// Providing parameters for motor control
A4988 stepper(spr, Dire, Step, MS1, MS2, MS3);

void setup() {
  Serial.begin(9600);
  pinMode(Step, OUTPUT); // Step pin as output
  pinMode(Dire, OUTPUT); // Direction pin as output
  pinMode(Sleep, OUTPUT); // Sleep pin as output
  digitalWrite(Step, LOW); // No stepper motor movement at start
  digitalWrite(Dire, LOW);
  digitalWrite(Sleep, LOW);
  
  // Set target motor RPM and microstepping setting
  stepper.begin(RPM, Microsteps);

  // Let the user know the system is ready
  Serial.println("Enter commands: rpm=[value], dir=[cw/ccw], startRotation, stopRotation");
}

void loop() {
  if (Serial.available() > 0) {
    String command = "";
    char incomingChar;
    while (Serial.available() > 0) {
      incomingChar = Serial.read();
      if (incomingChar == '\n' || incomingChar == '\r') {
        break;
      }
      command += incomingChar;
    }
    command.trim(); // Remove any leading/trailing whitespace

    if (command.length() == 0) {
      return; // Ignore empty commands
    }

    // Handle RPM setting
    if (command.startsWith("rpm=")) {
      RPM = command.substring(4).toInt();
      stepper.setRPM(RPM);
      Serial.print("RPM set to: ");
      Serial.println(RPM);
    }
    // Handle direction setting
    else if (command.startsWith("dir=")) {
      direction = command.substring(4);
      if (direction == "cw") {
        digitalWrite(Dire, LOW);
        Serial.println("Direction set to clockwise.");
      } else if (direction == "ccw") {
        digitalWrite(Dire, HIGH);
        Serial.println("Direction set to counterclockwise.");
      } else {
        Serial.println("Invalid direction. Use 'cw' or 'ccw'.");
      }
    }
    // Handle starting rotation
    else if (command.equals("startRotation")) {
      if (!isRotating) {
        digitalWrite(Sleep, HIGH);
        isRotating = true;
        Serial.println("Rotation started.");
      } else {
        Serial.println("Motor is already rotating.");
      }
    }
    // Handle stopping rotation
    else if (command.equals("stopRotation")) {
      if (isRotating) {
        digitalWrite(Sleep, LOW);
        isRotating = false;
        Serial.println("Rotation stopped.");
      } else {
        Serial.println("Motor is not rotating.");
      }
    } else {
      Serial.println("Invalid command.");
    }
  }

  // Motor control logic
  if (isRotating) {
    if (direction == "cw") {
      stepper.rotate(360);
    } else if (direction == "ccw") {
      stepper.rotate(-360);
    } else {
      stepper.rotate(360);
    }
  }
}
