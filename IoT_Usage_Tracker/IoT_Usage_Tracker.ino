// -------------------- LIB'S --------------------

#include <LiquidCrystal.h>
#include <Watchdog.h>
#include <Wire.h>

// -------------------- WATCHDOG --------------------

Watchdog watchdog;

// -------------------- CONSTANTS --------------------

#define MAX_ACCEL_VALUE 15200                   // Arbitrary units
#define DROP_ACCEL_MAGNITUDE_THRESHOLD 3        // G's. Magnitude must be > than this to be potentially a drop
#define DROP_ACCEL_AVG_MAGNITUDE_THRESHOLD 1.5   // G's. Average magnitude must be < this to be potentially a drop
#define DROP_TIME_INTERVAL_THRESHOLD 1          // Seconds. Last dropp must have occured at leas this long ago for the next drop to count

#define ACCELERATION_MAGNITUDE_SMOOTHING  100    // # of readings

// -------------------- IMU --------------------

#include "IMU.h"

float xAccel, yAccel, zAccel;

float currentMagnitude = 0;
float averageMagnitude = 0;
float totalAverageMagnitude = 0;
float maxMagnitude = 0;
int numberOfDrops = 0;

// -------------------- LCD --------------------

#define OUTPUT_INTERVAL 100                // Milliseconds
unsigned long lastOutputTime = 0;
boolean forceUpdateLCD = false;

//                RS EN D4 D5 D6 D7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// This is for the DFRobot LCD Keypad Sheild v1.1

// -------------------- SETUP --------------------

void setup() {

  // -------------------- SERIAL --------------------

  Serial.begin(115200);
  Serial.println("Starting...");

  // -------------------- WATCHDOG FOR MAIN PROGRAM --------------------

  watchdog.enable(Watchdog::TIMEOUT_250MS);

  // -------------------- IMU --------------------

  setupIMU();
  Serial.println("Started!");

  // -------------------- LCD --------------------

  setupLCD();

}


void loop() {

  // -------------------- WATCHDOG TIMER --------------------

  unsigned long currentMillis = millis();

  watchdog.reset();

  // -------------------- UPDATE IMU --------------------

  readIMUData();
  checkForDrops();

  // -------------------- OUTPUT DATA --------------------

  if (currentMillis - lastOutputTime > OUTPUT_INTERVAL || forceUpdateLCD) {
    lastOutputTime = currentMillis;
    forceUpdateLCD = false;

    // Refresh the LCD with new data
    updateLCD();

    // For serial plotter
    Serial.print("10,-10,");
    Serial.print(DROP_ACCEL_MAGNITUDE_THRESHOLD);
    Serial.print(",");
    Serial.print(currentMagnitude);
    Serial.print(F(", "));
    Serial.print(averageMagnitude);
    Serial.print(F(", "));
    Serial.print(maxMagnitude);
    Serial.print(F(", "));
    Serial.println(numberOfDrops);

  }

}

