// -------------------- LIB'S --------------------

#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

// -------------------- RFID READERS --------------------

#define RST_PIN         10          // Configurable, see typical pin layout above
#define SS_1_PIN        9         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
#define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

#define TAG_READER   0
#define ASSET_READER 1

#define NUMBER_OF_CARD_PRESENT_CHECKS 5

byte ssPins[] = {SS_1_PIN, SS_2_PIN};
MFRC522 mfrc522[2];   // Create MFRC522 instance.

// -------------------- SERVO --------------------

#define SERVO_PIN A3

Servo doorServo;

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

  // -------------------- RFID READERS --------------------
  
  doorServo.attach(SERVO_PIN);
  doorServo.write(0);

}

/**
   Main loop.
*/
void loop() {

  /*
    Serial.print("Status Tag, Asset: ");
    Serial.print(getCardStatus(TAG_READER));
    Serial.print(", ");
    Serial.println(getCardStatus(ASSET_READER));
  */

  static boolean lastCardStatus = false;

  if (mfrc522[TAG_READER].PICC_IsNewCardPresent() && mfrc522[TAG_READER].PICC_ReadCardSerial()) {

    if (getCardStatus(ASSET_READER)) {
      Serial.print(F("out,"));
    } else {
      Serial.print(F("in,"));
    }
    dump_byte_array(mfrc522[TAG_READER].uid.uidByte, mfrc522[TAG_READER].uid.size);
    Serial.println();

    doorServo.write(160);

    // Halt PICC
    mfrc522[TAG_READER].PICC_HaltA();
    // Stop encryption on PCD
    mfrc522[TAG_READER].PCD_StopCrypto1();
  }

/*
  if(getCardStatus(TAG_READER)) {  
    doorServo.write(160);
  } else {
    doorServo.write(0);
  }
*/

}

int getCardStatus(int readerIndex) {

  for (int i = 0; i < NUMBER_OF_CARD_PRESENT_CHECKS; i++) {
    if (mfrc522[readerIndex].PICC_IsNewCardPresent()) {
      return true;
    }
  }

  return false;

}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}
