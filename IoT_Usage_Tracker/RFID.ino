void checkToLogDrop() {

  static int lastNumberOfDrops = numberOfDrops;

  if (lastNumberOfDrops != numberOfDrops) {

    byte buffer[16];
    for (byte i = 0; i < 16; i++) buffer[i] = 0;
    byte block = 9;
    MFRC522::StatusCode status;

    while (!foundCard) {
      //watchdog.reset();
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        foundCard = true;
      }
    }

    /*
    byte bufferATQA[2];
    byte bufferSize = sizeof(bufferATQA);
    status = mfrc522.PICC_WakeupA(bufferATQA, bufferSize);
    */
    
    buffer[0] = numberOfDrops;

    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    //mfrc522.PICC_HaltA(); // Halt PICC
    //mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  }

  lastNumberOfDrops = numberOfDrops;

}

