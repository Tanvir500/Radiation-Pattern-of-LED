#include <Stepper.h>
#include <BH1750.h>
#include <Wire.h>

// Stepper Motor Settings
const int stepsPerRevolution = 100; // Number of steps per revolution for your motor
const float stepAngle = 180.0 / stepsPerRevolution; // Degrees per step
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11); // Motor pins

// Light Sensor Settings
BH1750 lightMeter;

// Measurement Settings
const int numMeasurements = 50; // Number of light readings per step
const int stepDelayMs = 510;    // Delay after each step (ms)
const int measureDelayMs = 10;  // Delay between light readings (ms)

// Variables
float currentAngle = 0.0;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  Serial.println("Arduino Light Measurement and Motor Control");

  // Initialize I2C and BH1750
  Wire.begin();
  if (!lightMeter.begin()) {
    Serial.println("Error initializing BH1750 sensor");
    while (1); // Halt execution if sensor initialization fails
  }
  Serial.println("BH1750 sensor initialized");

  // Initialize Stepper motor
  myStepper.setSpeed(5); // Set motor speed (RPM)

  // Notify MATLAB that Arduino is ready
  Serial.println("READY");

  // Wait for "RUN" command from MATLAB
  while (true) {
    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      command.trim();
      if (command == "RUN") {
        Serial.println("RUN RECEIVED");
        break; // Exit loop and proceed
      }
    }
    delay(100); // Avoid busy-waiting
  }
}

void loop() {
  for (int stepCount = 0; stepCount < stepsPerRevolution; ++stepCount) {
    // Move one step
    myStepper.step(-1);
    currentAngle += stepAngle;

    delay(stepDelayMs); // Wait for motor to stabilize

    // Take multiple light readings and calculate average
    float lightSum = 0.0;
    for (int i = 0; i < numMeasurements; ++i) {
      lightSum += lightMeter.readLightLevel();
      delay(measureDelayMs);
    }
    float lightAverage = lightSum / numMeasurements;

    // Send data to Serial
    Serial.print(stepCount + 1); // Step number
    Serial.print(", ");
    Serial.print(currentAngle, 2); // Angle in degrees
    Serial.print(", ");
    Serial.println(lightAverage, 2); // Average light in lx
  }

  // Return to the initial position
  myStepper.step(stepsPerRevolution);
  Serial.println("Measurement complete.");
  while (true) {
    delay(1000); // Halt execution
  }
}
