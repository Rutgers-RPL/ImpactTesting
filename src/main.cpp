#include <Arduino.h>
//#include <BMI085.h>
#include "SparkFun_LIS331.h"
#include <Wire.h>
#include <Quaternion.h>
#include <SdFat.h>
#include <SD.h>

#define _g_ (9.80665)
#define FILE_BASE_NAME "ImpactData_"
bool sdexists = false;
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char* fileName = FILE_BASE_NAME "0000.csv";
int count = 0;
  
FsFile f;
SdFs sd;
LIS331 xl;

void setup() {
  //Begin set up of LIS331
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
  //End set up of LIS331. Code came from https://learn.sparkfun.com/tutorials/h3lis331dl-accelerometer-breakout-hookup-guide#library-overview

  Serial.begin(115200);
  
  //Setting up SD card
  
  if (!sd.begin(SdioConfig(FIFO_SDIO))) { 
    Serial.println("SD Begin Failed");
  } else {
    Serial.println("\nFIFO SDIO mode.");
    while (sd.exists(fileName)) {
        if (fileName[BASE_NAME_SIZE + 3] != '9') {
              fileName[BASE_NAME_SIZE + 3]++;
        } else if (fileName[BASE_NAME_SIZE + 2] != '9') {
              fileName[BASE_NAME_SIZE + 3] = '0';
              fileName[BASE_NAME_SIZE + 2]++;
        } else if (fileName[BASE_NAME_SIZE + 1] != '9') {
              fileName[BASE_NAME_SIZE + 2] = '0';
              fileName[BASE_NAME_SIZE + 3] = '0';
              fileName[BASE_NAME_SIZE + 1]++;
        } else if (fileName[BASE_NAME_SIZE] != '9') {
              fileName[BASE_NAME_SIZE + 1] = '0';
              fileName[BASE_NAME_SIZE + 2] = '0';
              fileName[BASE_NAME_SIZE + 3] = '0';
              fileName[BASE_NAME_SIZE]++;
        } else {
              Serial.println("Can't create file name");
        }
    }
    f = sd.open(fileName, FILE_WRITE);
    Serial.print("Writing to: ");
    Serial.println(fileName);
    if (!f) {
        Serial.println("Failed opening file.");
    }
    sdexists = true;
  } //End setting up SD card
}

void loop() {
  
  int16_t x; int16_t y; int16_t z;
  xl.readAxes(x, y, z);
  x = xl.convertToG(400,x) * _g_; //Acceleration in the x in terms of m/s^2
  y = xl.convertToG(400,y) * _g_; //Acceleration in the y in terms of m/s^2
  z = xl.convertToG(400,z) * _g_; //Acceleration in the z in terms of m/s^2

  if (digitalRead(9) == HIGH)
  {
    Serial.println("Interrupt");
  }
  // Printing to SD Card
  if (sdexists && f) {
    f.print(x); f.print(","); f.print(y); f.print(","); f.println(z);
  } else {
    Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(","); Serial.println(z);
  }

  if (count % 15 == 0) {

    if (sdexists && f) {
      f.close();
      f = sd.open(fileName, FILE_WRITE);
    }
  }

  count += 1;

  
}