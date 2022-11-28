#include <Arduino.h>
//#include <BMI085.h>
#include "SparkFun_LIS331.h"
#include <Wire.h>
#include <Quaternion.h>
#include <SdFat.h>
#include <SD.h>

#define _g_ (9.80665)
#define FILE_BASE_NAME "FlightLog_"
LIS331 xl;

void setup() {
  
  pinMode(9,INPUT);       // Interrupt pin input
  Wire.begin();
  xl.setI2CAddr(0x19);    // This MUST be called BEFORE .begin() so 
                          //  .begin() can communicate with the chip

  xl.begin(LIS331::USE_I2C); // Selects the bus to be used and sets
                          //  the power up bit on the accelerometer.
                          //  Also zeroes out all accelerometer
                          //  registers that are user writable.
  // This next section configures an interrupt. It will cause pin
  //  INT1 on the accelerometer to go high when the absolute value
  //  of the reading on the Z-axis exceeds a certain level for a
  //  certain number of samples.

  xl.intSrcConfig(LIS331::INT_SRC, 1); // Select the source of the
                          //  signal which appears on pin INT1. In
                          //  this case, we want the corresponding
                          //  interrupt's status to appear. 
  xl.setIntDuration(50, 1); // Number of samples a value must meet
                          //  the interrupt condition before an
                          //  interrupt signal is issued. At the
                          //  default rate of 50Hz, this is one sec.

  xl.setIntThreshold(2, 1); // Threshold for an interrupt. This is
                          //  not actual counts, but rather, actual
                          //  counts divided by 16.

  xl.enableInterrupt(LIS331::Z_AXIS, LIS331::TRIG_ON_HIGH, 1, true);
                          // Enable the interrupt. Parameters indicate
                          //  which axis to sample, when to trigger
                          //  (in this case, when the absolute mag
                          //  of the signal exceeds the threshold),
                          //  which interrupt source we're configuring,
                          //  and whether to enable (true) or disable
                          //  (false) the interrupt.
  Serial.begin(115200);
}

void loop() {
  
  int16_t x; int16_t y; int16_t z;
  xl.readAxes(x, y, z);
  x = xl.convertToG(200,x) * _g_; //Acceleration in the x in terms of m/s^2
  y = xl.convertToG(200,y) * _g_; //Acceleration in the y in terms of m/s^2
  z = xl.convertToG(200,z) * _g_; //Acceleration in the z in terms of m/s^2

  if (digitalRead(9) == HIGH)
  {
    Serial.println("Interrupt");
  }

  bool sdexists = false;
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char* fileName = FILE_BASE_NAME "0000.csv";
  
  FsFile f;
  SdFs sd;

  
}