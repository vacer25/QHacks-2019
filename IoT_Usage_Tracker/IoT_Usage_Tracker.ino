// -------------------- LIB'S --------------------

#include <LiquidCrystal.h>
#include <Watchdog.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

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

// -------------------- RFID --------------------

#define RST_PIN         2
#define SS_PIN          3

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;

boolean foundCard = false;
boolean foundCardSerial = false;

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

  // -------------------- WATCHDOG FOR MAIN PROGRAM --------------------

  watchdog.enable(Watchdog::TIMEOUT_4S);

  // -------------------- LCD --------------------

  setupLCD();

  // -------------------- RFID --------------------

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;


}


void loop() {

  // -------------------- WATCHDOG TIMER --------------------

  unsigned long currentMillis = millis();

  watchdog.reset();

  /*
    // Look for new cards
    if (!foundCard && !mfrc522.PICC_IsNewCardPresent()) {
    Serial.println(F("No new cards (loop)"));
    foundCard = true;
    return;
    }

    // Select one of the cards
    if (!foundCardSerial && !mfrc522.PICC_ReadCardSerial()) {
    Serial.println(F("No serial cards (loop)"));
    foundCardSerial = true;
    return;
    }
  */

  // -------------------- UPDATE --------------------

  readIMUData();
  checkForDrops();
  checkToLogDrop();

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

