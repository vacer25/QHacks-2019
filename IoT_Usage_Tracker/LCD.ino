void updateLCD() {

  /*
    // Usage time on top
    lcd.setCursor(0, 0);
    lcd.print("Usage time: WIP");
  */

  // Current force
  lcd.setCursor(0, 0);
  lcd.print("C: ");
  lcd.setCursor(3, 0);
  lcd.print(currentMagnitude, 2);

  // Average force
  lcd.setCursor(8, 0);
  lcd.print("A: ");
  lcd.setCursor(11, 0);
  lcd.print(totalAverageMagnitude, 2);

  // Max force
  lcd.setCursor(0, 1);
  lcd.print("M: ");
  lcd.setCursor(3, 1);
  lcd.print(maxMagnitude, 2);

  // Number of drops on bottom
  lcd.setCursor(8, 1);
  lcd.print("D: ");
  lcd.setCursor(11, 1);
  lcd.print(numberOfDrops);

}

void setupLCD() {
  lcd.begin(16, 2);
}
