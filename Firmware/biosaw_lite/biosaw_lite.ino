// biosaw_lite.ino
// Plan B biosaw code

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

#define red_led 3
#define green_led 5
#define blue_led 6

#define rst_dds 2
#define io_update 4



void setup() {
  Serial.begin(9600);
  Serial.println("Startup...");
  delay(100); // Wait for all devices to turn on.

  // Check DDS.
  DDS_START();

  
  Serial.println("Startup sucessful!");
}

void loop() {
  
}

int DDS_START(){

}
}
