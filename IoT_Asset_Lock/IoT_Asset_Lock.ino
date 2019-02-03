// -------------------- LIB'S --------------------

#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

// -------------------- RFID READERS --------------------

#define RST_PIN         10          // Configurable, see typical pin layout above
#define SS_1_PIN        9         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
#define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

#define TAG_READER      0
#define ASSET_READER    1

#define NUMBER_OF_CARD_PRESENT_CHECKS 5

byte ssPins[] = {SS_1_PIN, SS_2_PIN};
MFRC522 mfrc522[2];   // Create MFRC522 instance.

byte lastTagUID[10];
byte lastTagNumerOfBytes;

// -------------------- BUTTON --------------------

#define CLOSE_DOOR_AFTER_ASSET_PUT_IN_TIME 5000       // ms
#define CLOSE_DOOR_AFTER_ASSET_TAKEN_OUT_TIME 5000    // ms
#define CLOSE_DOOR_IF_ASSET_NOT_TAKEN_OUT_TIME 10000  // ms
#define CLOSE_DOOR_IF_ASSET_NOT_PUT_IN_TIME 10000     // ms
#define CLOSE_DOOR_DELAY  500                         // ms

#define BUTTON_PIN 7
boolean assetIsIn = true;
unsigned long assetTakenOutTime = 0;
unsigned long assetPutInTime = 0;

boolean assetIsBeingTakenOut = false;

// -------------------- SERVO --------------------

#define SERVO_PIN A3

#define DOOR_OPEN_ANGLE 160 // Degrees
#define DOOR_CLOSE_ANGLE 0  // Degrees

Servo doorServo;
boolean doorIsOpen = false;
unsigned long doorOpenTime = 0;

// -------------------- SETUP --------------------

void setup() {

  // -------------------- SERIAL --------------------

  Serial.begin(115200); // Initialize serial communications with the PC

  // -------------------- RFID READERS --------------------

  for (uint8_t reader = 0; reader < 2; reader++) {
    pinMode(ssPins[reader], OUTPUT);
    digitalWrite(ssPins[reader], HIGH);
  }

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < 2; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
  }

  // -------------------- SERVO --------------------

  doorServo.attach(SERVO_PIN);
  closeDoor();

  // -------------------- BUTTON --------------------

  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

/**
   Main loop.
*/
void loop() {

  unsigned long currentMillis = millis();

  if (!doorIsOpen) {

    // When a tag is detected
    if (mfrc522[TAG_READER].PICC_IsNewCardPresent() && mfrc522[TAG_READER].PICC_ReadCardSerial()) {

      // Determine if asset is being put in or taken out
      if (getCardStatus(ASSET_READER)) {
        assetIsBeingTakenOut = true;
        assetIsIn = true;
      } else {
        assetIsBeingTakenOut = false;
        assetIsIn = false;
      }

      // Open the door
      openDoor(currentMillis);

      // Halt PICC & Stop encryption on PCD
      mfrc522[TAG_READER].PICC_HaltA();
      mfrc522[TAG_READER].PCD_StopCrypto1();

    }

  }

  if (assetIsBeingTakenOut) {

    // If the asset was just taken out, record the time that happened
    if (doorIsOpen && assetIsIn) {
      if (digitalRead(BUTTON_PIN)) {
        assetTakenOutTime = currentMillis;
        assetIsIn = false;
      }
    }

    // If enough time has passed since the asset was taken out, close the door
    if (doorIsOpen && !assetIsIn && currentMillis - doorOpenTime > CLOSE_DOOR_AFTER_ASSET_TAKEN_OUT_TIME) {
      closeDoor();
      //Serial.println("Closed (1)");

      // Determine if the asset was actually taken out
      if (!getCardStatus(ASSET_READER)) {
        // Print info to PC
        Serial.print("out,");
        dump_byte_array(mfrc522[TAG_READER].uid.uidByte, mfrc522[TAG_READER].uid.size);
        Serial.println();
      }

    }

    // If the door has been open for too long and the asset not taken out, close the door
    if (doorIsOpen && assetIsIn && currentMillis - assetTakenOutTime > CLOSE_DOOR_IF_ASSET_NOT_TAKEN_OUT_TIME) {
      closeDoor();
      //Serial.println("Closed (2)");
    }

  } else {

    // If the asset was put in, record the time that happened
    if (!assetIsIn && !digitalRead(BUTTON_PIN)) {
      assetIsIn = true;
      assetPutInTime = currentMillis;
    }

    // If enough time has passed since the asset was put in, close the door
    if (doorIsOpen && assetIsIn && currentMillis - assetPutInTime > CLOSE_DOOR_AFTER_ASSET_PUT_IN_TIME) {
      closeDoor();
      //Serial.println("Closed (3)");

      // Determine if the asset was actually put in
      if (getCardStatus(ASSET_READER)) {
        // Print info to PC
        Serial.print("in,");
        dump_byte_array(mfrc522[TAG_READER].uid.uidByte, mfrc522[TAG_READER].uid.size);
        Serial.println();
      }

    }

    // If the door has been open for too long and the asset not put in, close the door
    if (doorIsOpen && !assetIsIn && currentMillis - doorOpenTime > CLOSE_DOOR_IF_ASSET_NOT_PUT_IN_TIME) {
      closeDoor();
      //Serial.println("Closed (4)");
    }

  }

}

void closeDoor() {
  doorServo.write(DOOR_CLOSE_ANGLE);
  doorIsOpen = false;
  delay(CLOSE_DOOR_DELAY);
}

void openDoor(unsigned long currentMillis) {
  doorServo.write(DOOR_OPEN_ANGLE);
  doorIsOpen = true;
  doorOpenTime = currentMillis;
  assetTakenOutTime = currentMillis;
}

int getCardStatus(int readerIndex) {

  for (int i = 0; i < NUMBER_OF_CARD_PRESENT_CHECKS; i++) {
    if (mfrc522[readerIndex].PICC_IsNewCardPresent()) {
      return true;
    }
  }

  return false;

}

// Helper routine to dump a byte array as hex values to Serial.
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}
