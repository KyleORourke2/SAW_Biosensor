// biosaw_lite.ino
// Plan B biosaw code

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <AD9959.h>
#include <SPI.h>

const int red_led = 3;
const int green_led = 5;
const int blue_led = 6;

const int CS = 10;       // Chip select
const int rst_dds = 2;   // DDS reset pin.
const int io_update = 4; // DDS I/O update toggle.
const int P0 = 8;        // Sweep control pin

class MyAD9959 : public AD9959<
    rst_dds,        // Reset pin (active = high)
    CS,             // Chip Enable (active = low)
    io_update,      // I/O_UPDATE: Apply config changes (pulse high)
    25000000,       // 25MHz crystal (optional)
    1000000         // SPI rate 1MBit/s
> {};
MyAD9959  dds;

void setup() {
  Serial.begin(9600);
  Serial.println("Startup...");

  pinMode(CS, OUTPUT);
  pinMode(rst_dds, OUTPUT);
  pinMode(io_update, OUTPUT);
  pinMode(P0, OUTPUT);
  pinMode(blue_led, OUTPUT);
  delay(500); // Wait for all devices to turn on.
  digitalWrite(blue_led, HIGH);
  
  // Check DDS.
  dds.reset();
  dds.setClock(20, 0);
  delay(10);
  uint32_t  delta;
  delta = dds.frequencyDelta(455000);
  delay(10);
  dds.setFrequency(MyAD9959::Channel3, 10000000UL);
  dds.update();

  
  Serial.println("Startup Finished.");
}

void loop() {

  delay(100);
}
