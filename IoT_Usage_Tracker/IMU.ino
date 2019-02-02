float getAccelerationMagnitude() {
  return sqrt(pow(xAccel, 2) + pow(yAccel, 2) + pow(zAccel, 2));
}

float checkForDrops() {

  unsigned long currentMillis = millis();
  static unsigned long timeOfLastDrop = 1;

  float currentAccelerationMagnitude = getAccelerationMagnitude();
  static float previousAccelerationMagnitude = 0;

  // If the acceleration is over the threshold and was not last time
  if (currentAccelerationMagnitude > DROP_ACCEL_MAGNITUDE_THRESHOLD && previousAccelerationMagnitude < DROP_ACCEL_MAGNITUDE_THRESHOLD) {

    // If the average magnitude is below a threshold
    if (averageMagnitude < DROP_ACCEL_AVG_MAGNITUDE_THRESHOLD) {

      // If enough time since the last drop occured
      if (currentMillis - timeOfLastDrop > (DROP_TIME_INTERVAL_THRESHOLD * 1000)) {

        // A drop occured
        numberOfDrops++;
        timeOfLastDrop = currentMillis;
        forceUpdateLCD = true;

      }

    }

  }

  // Save previous acceleration magnitude
  previousAccelerationMagnitude = currentAccelerationMagnitude;

}

void readIMUData() {

  int error;
  double dT;

  //Serial.println(F(""));
  //Serial.println(F("MPU-6050"));

  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.
  error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));

  /*

    Serial.print(F("Read accel, temp and gyro, error = "));
    Serial.println(error,DEC);

  */

  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
#define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

  // Process data
  xAccel = accel_t_gyro.value.x_accel != 32767 ? (float)accel_t_gyro.value.x_accel / MAX_ACCEL_VALUE : xAccel;
  yAccel = accel_t_gyro.value.y_accel != 32767 ? (float)accel_t_gyro.value.y_accel / MAX_ACCEL_VALUE : yAccel;
  zAccel = accel_t_gyro.value.z_accel != 32767 ? (float)accel_t_gyro.value.z_accel / MAX_ACCEL_VALUE : zAccel;

  // Get current magnitude
  currentMagnitude = getAccelerationMagnitude();

  // Calculate average magnitude
  accelerationReadingsSumValue -= accelerationReadings[accelerationReadingIndex];
  accelerationReadings[accelerationReadingIndex] = currentMagnitude;
  accelerationReadingsTotalValue += currentMagnitude;
  accelerationReadingsSumValue += accelerationReadings[accelerationReadingIndex];
  accelerationReadingIndex++;
  accelerationTotalReadingIndex++;
  if (accelerationReadingIndex >= ACCELERATION_MAGNITUDE_SMOOTHING) {
    accelerationReadingIndex = 0;
  }
  if (isFirstAccelerationReading) {
    isFirstAccelerationReading = false;
    averageMagnitude = currentMagnitude;
  }
  else {
    averageMagnitude = accelerationReadingsSumValue / ACCELERATION_MAGNITUDE_SMOOTHING;
  }
  totalAverageMagnitude = accelerationReadingsTotalValue / accelerationTotalReadingIndex;

  // Record max magnitude
  if (currentMagnitude > maxMagnitude) {
    maxMagnitude = currentMagnitude;
  }


}

void setupIMU() {

  int error;
  uint8_t c;

  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();

  // default at power-up:
  //    Gyro at 250 degrees second
  //    Acceleration at 2g
  //    Clock source at internal 8MHz
  //    The device is in sleep mode.
  //

  error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
  Serial.print(F("WHO_AM_I : "));
  Serial.print(c, HEX);
  Serial.print(F(", error = "));
  Serial.println(error, DEC);

  // According to the datasheet, the 'sleep' bit
  // should read a '1'.
  // That bit has to be cleared, since the sensor
  // is in sleep mode at power-up.
  error = MPU6050_read (MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print(F("PWR_MGMT_1 : "));
  Serial.print(c, HEX);
  Serial.print(F(", error = "));
  Serial.println(error, DEC);


  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);

}

// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size) {
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while (Wire.available() && i < size)
  {
    buffer[i++] = Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size) {
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data) {
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}
