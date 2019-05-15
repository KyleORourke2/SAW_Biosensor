// biosaw_lite.ino
// Biolite Firmware

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

#define ref_clk 25000000
#define sys_clk 500000000
#define max32   4294967296

#define CSR  0x00
#define FR1  0x01
#define FR2  0x02
#define CFR  0x03
#define CFTW 0x04


const int rst_dds = 2;   // DDS reset pin.
const int io_update = 4; // DDS I/O update toggle.
const int P0 = 8;        // Sweep control pin

const int CS = 10;       // Chip select
//const int MOSI = 11;       // SPI MOSI
//const int MISO = 12;       // SPI MISO
//const int SCK = 13;       // SPI SCK

const int red_led = 3;   // RGB LED
const int green_led = 5;
const int blue_led = 6;

void setup() {
  Serial.begin(115200);
  SPI.begin();
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
  dleds(0,0,0);
  //leds(0, 20, 10); // Turn on LED for visual verification.
  digitalWrite(P0, LOW);  // Ensure profile pin 0 low.
  digitalWrite(CS, HIGH); // Ensure slave select high
  

  dds_setup();
  updateChannelFreqs(50000000);
  
  Serial.println("Startup Finished.");
}

void loop() {
  //updateChannelFreqs(10000000);
}


// Set control registers. From AD9959 datasheet Pg.36 Table 28.
int dds_setup(){
  
  // Reset DDS
  ddsReset();

  // Channel Select Register: CSR (0x00)
  ddsWrite_8(CSR, 0x82); // Set Ch.3 EN, I/O mode 3-wire is 01 (10000010)
  
  // Function Register 1: FR1 (0x01)
  // VCO gain[7], PLL divider[6:2], Charge pump[1:0]
  // Open[7], PPC[6:4], RU/RD[3:2], Mod[1:0]
  // RefClk[7], ExtPwrDwn[6], SYNC_CLK disable[5], ...
  ddsWrite_24(FR1, 0xD00020); // 1101 0000 0000 0000 0010 0000
  
  // Function Register 2: FR2 (0x02)
  // ddsWrite_16(FR2, 0x0000); // Default values.

  // Channel Function Register: CFR (0x03)
  // ddsWrite_24(CFR, 0x000302); // Default values.
  
  delay(10); // Wait for system to settle.
}


// Sets the frequency of a given channel.
void updateChannelFreqs(long RFfreq) { 
  ddsWrite_8(CSR, 0x82); // Set Ch.3 EN, I/O mode 3-wire is 01 (1000 0010)
  long ftwRF = MHzToFTW(RFfreq);
  ddsWrite_32(CFTW, ftwRF);  //increment the RF channel FTW
  pulse(io_update);

  //ddsWrite_8(CSR, 0x02);  //close off channels
}

long MHzToFTW(long MHzFrequency) {         //given a freq, return a 32-bit FTW
  long FTWconst = (sys_clk / max32); //define freq <--> FTW conversion constant
  return (long) MHzFrequency / FTWconst;   //return FTW
}

long FTWtoMHz(long FTW) {                 //given a 32-bit FTW, return a freq
  long FTWconst = (sys_clk / max32); //define freq <--> FTW conversion constant
  return (long) FTW * FTWconst;
}

//sweepDDS is basically the actual main() below:
/*
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
*/

void ddsReset(){
  pulse(rst_dds); // Pulse reset pin.
  //pulse(io_update); // Pulse IO update.
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
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS, LOW);  // Enable slave select.
  delay(1);               // Wait for chip to enable.
}

void spiEnd(){
  digitalWrite(CS, HIGH);  // Disable slave select.
  SPI.endTransaction();
}

// Pulses given pin.
void pulse(uint8_t pin){
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
  delay(1);
  digitalWrite(pin, LOW);
}

// Simple LED control function
void leds(int R, int G, int B) {
  analogWrite(red_led, 255-R);
  analogWrite(green_led, 255-G);
  analogWrite(blue_led, 255-B);
}

// Simple LED control function
void dleds(int R, int G, int B) {
  digitalWrite(red_led, 1-R);
  digitalWrite(green_led, 1-G);
  digitalWrite(blue_led, 1-B);
}
