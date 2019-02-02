// -------------------- LIB'S --------------------

#include <Wire.h>
#include "IMU.h"

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

  // -------------------- SERIAL --------------------

  Serial.begin(115200);
  Serial.println("Starting...");

  // -------------------- IMU --------------------

  setupIMU();
  Serial.println("Started!");

}


void loop() {

  unsigned long currentMillis = millis();

  // -------------------- UPDATE IMU --------------------

  readIMUData();
  checkForDrops();

  // -------------------- OUTPUT DATA --------------------

  //if (currentMillis - lastOutputTime > OUTPUT_INTERVAL) {
    //lastOutputTime = currentMillis;

    static float maxXYZ = 0;
    if(xAccel > maxXYZ) {
      maxXYZ = xAccel;
    }
    if(yAccel > maxXYZ) {
      maxXYZ = yAccel;
    }
    if(zAccel > maxXYZ) {
      maxXYZ = zAccel;
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
    Serial.print(getAccelerationSquaredMagnitude());
    Serial.print(F(", "));
    Serial.println(numberOfDrops);

  //}

}

