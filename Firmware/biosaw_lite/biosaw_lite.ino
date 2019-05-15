// biosaw_lite.ino
// Biolite Firmware

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

#define CSR 0x00
#define FR1 0x01
#define FR2 0x02
#define CFR 0x03
#define CFW 0x04

const int CS = 10;       // Chip select
const int rst_dds = 2;   // DDS reset pin.
const int io_update = 4; // DDS I/O update toggle.
const int P0 = 8;        // Sweep control pin

const int red_led = 3;   // RGB LED
const int green_led = 5;
const int blue_led = 6;

void setup() {
  Serial.begin(115200);
  Serial.println("BioLite Startup...");

  // Set pinModes:
  pinMode(CS, OUTPUT);
  pinMode(rst_dds, OUTPUT);
  pinMode(io_update, OUTPUT);
  pinMode(P0, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);

  // Set init pin values:
  leds(0, 20, 10); // Turn on LED for visual verification.
  digitalWrite(P0, LOW);  // Ensure profile pin 0 low.
  digitalWrite(CS, HIGH); // Ensure slave select high
  SPI.begin();


  Serial.println("Startup Finished.");
}

void loop() {

}


// Set control registers. From AD9959 datasheet Pg.36 Table 28.
int dds_setup(){
  
  // Channel Select Register:
  ddsWrite_8(CSR, 0x82); // Set Ch.3 EN, I/O mode 3-wire is 01
                         // (10000010)
  
  // Function Register 1:
  ddsWrite_24(FR1, 0x00D00020);
  // VCO gain[7], PLL divider[6:2], Charge pump[1:0]
  // Open[7], PPC[6:4], RU/RD[3:2], Mod[1:0]
  // RefClk[7], ExtPwrDwn[6], SYNC_CLK disable[5], ...

  // Channel Function Register:
  ddsWrite_24();
}



// Simple function for writing 8 bit values
// 1 byte = 8 bits (Arduino)
// 1 hex val = 4 bits
void ddsWrite_8(byte address, byte val){
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(val);
  spiEnd();
}

// Simple function for writing 16 bit values
void ddsWrite_16(byte address, int val){
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(&val, 2);
  spiEnd();
}

// Simple function for writing 3 byte values
void ddsWrite_24(byte address, long val){
  byte a,b,c;
  a=(val      &0xFF); //extract first byte
  b=((val>>8) &0xFF); //extract second byte
  c=((val>>16)&0xFF); //extract third byte
  
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(c); // We want to transfer MSB first.
  SPI.transfer(b);
  SPI.transfer(a);
  spiEnd();
}

// Simple function for writing 4 byte values
void ddsWrite_32(byte address, long val){
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(&val, 4);
  spiEnd();
}

void spiBegin(){
  digitalWrite(CS, LOW);  // Enable slave select.
  delay(1);               // Wait for chip to enable.
}

void spiEnd(){
  digitalWrite(CS, HIGH);  // Disable slave select.
}

// Simple LED control function
void leds(int R, int G, int B) {
  analogWrite(red_led, 255-R);
  analogWrite(green_led, 255-G);
  analogWrite(blue_led, 255-B);
}

void updateChannelFreqs(long RFfreq, long LOfreq) {  
  long ftwRF = MHzToFTW(RFfreq);
  long ftwLO = MHzToFTW(LOfreq);
  byte channelSelect = 16;        //define channelSelect as an 8-bit number with bit 4 hot
  ddsWrite_8(0x00, channelSelect);   //write bit 4 of 0x00 to select channel 0
  ddsWrite_32(0x04, ftwRF);          //increment the RF channel FTW
  channelSelect = 0;
  ddsWrite_8(0x00, channelSelect);  //close off channels
  channelSelect = 32;                //set Channelselect as an 8-bit number with bit 5 hot
  ddsWrite_8 (0x00, channelSelect);  //write bit 5 of 0x00 to select channel 1
  ddsWrite_32(0x04, ftwLO);          //increment the LO channel FTW
  channelSelect = 0;
  ddsWrite_8 (0x00, channelSelect);  //close off channels
}

long MHzToFTW(long MHzFrequency) {         //given a freq, return a 32-bit FTW
  long FTWconst = (500000000 / 4294967296); //define freq <--> FTW conversion constant
  return (long) MHzFrequency / FTWconst;   //return FTW
}

long FTWtoMHz(long FTW) {                 //given a 32-bit FTW, return a freq
  long FTWconst = (500000000 / 4294967296); //define freq <--> FTW conversion constant
  return (long) FTW * FTWconst;
}

//sweepDDS is basically the actual main() below:
void sweepDDS(int punchIt)  {
  //first set starting frequency and other necessary things:
  long RFfreq = 40000000;
  long LOfreq = 39999000;
  updateChannelFreqs(RFfreq, LOfreq);     //set RF, LO channels to their  starting freqs

  while (RFfreq < 60000000) {           //while frequency is less than 60MHz,
    delay(1);
    if (punchIt == 1) {
      RFfreq += 1000; //increment RF channel frequency
      LOfreq += 1000; //increment LO channel frequency
      updateChannelFreqs(RFfreq, LOfreq); // update up RF and LO channel freqs
      // sample & write ADC output to memory (!!!KYLE WHAT DO I DO!)
      // look up analog read example for arduino.
      // look up "processing" for arduino.
    }
  }
  Serial.println("got out of while loop in sweepDDS. Show is over.");
}
