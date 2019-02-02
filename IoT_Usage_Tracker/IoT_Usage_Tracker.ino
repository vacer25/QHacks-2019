// -------------------- LIB'S --------------------

#include <Watchdog.h>
#include <Wire.h>
#include "IMU.h"

// -------------------- WATCHDOG --------------------

Watchdog watchdog;

// -------------------- CONSTANTS --------------------

#define OUTPUT_INTERVAL 10                // Milliseconds
unsigned long lastOutputTime = 0;

#define MAX_ACCEL_VALUE 20000             // Arbitrary units
#define DROP_ACCEL_MAGNITUDE_THRESHOLD 8  // Arbitrary units
#define DROP_TIME_INTERVAL_THRESHOLD 1    // Seconds

// -------------------- DATA --------------------

float xAccel, yAccel, zAccel;
int numberOfDrops = 0;

// -------------------- SETUP --------------------

void setup() {

  watchdog.reset();

  // -------------------- SERIAL --------------------

  Serial.begin(115200);
  Serial.println("Starting...");

  // -------------------- WATCHDOG TIMER --------------------

  watchdog.enable(Watchdog::TIMEOUT_2S);

  // -------------------- IMU --------------------

  setupIMU();
  Serial.println("Started!");

  watchdog.reset();
  watchdog.enable(Watchdog::TIMEOUT_250MS);

}


void loop() {

  // -------------------- WATCHDOG TIMER --------------------

  unsigned long currentMillis = millis();

  watchdog.reset();

  // -------------------- UPDATE IMU --------------------

  readIMUData();
  checkForDrops();

  // -------------------- OUTPUT DATA --------------------

  if (currentMillis - lastOutputTime > OUTPUT_INTERVAL) {
    lastOutputTime = currentMillis;

    static float maxMagnitude = 0;
    float currentMagnitude = getAccelerationSquaredMagnitude();
    if (currentMagnitude > maxMagnitude) {
      maxMagnitude = currentMagnitude;
    }

    /*
      // For serial monitor
      Serial.print(F("X: "));
      Serial.print(xAccel);
      Serial.print(F("\tY: "));
      Serial.print(yAccel);
      Serial.print(F("\tZ: "));
      Serial.print(zAccel);
      Serial.print(F("\tM: "));
      Serial.print(maxXYZ);
      Serial.println();
    */

    // For serial plotter
    Serial.print("10,-10,");
    Serial.print(DROP_ACCEL_MAGNITUDE_THRESHOLD);
    Serial.print(",");
    Serial.print(currentMagnitude);
    Serial.print(F(", "));
    Serial.println(numberOfDrops);

  }

}

