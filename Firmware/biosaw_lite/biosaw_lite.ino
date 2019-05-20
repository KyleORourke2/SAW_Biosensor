// biosaw_lite.ino
// Biolite Firmware

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

#define ref_clk 25000000
#define sys_clk 25000000 //we want 500000000, testing 25000000 for now
#define max32   4294967296

const byte CSR  = 0x00;
const byte FR1  = 0x01;
const byte FR2  = 0x02;
const byte CFR  = 0x03;
const byte CFTW = 0x04;


const int rst_dds = 2;   // DDS reset pin.
const int io_update = 4; // DDS I/O update toggle.
const int P0 = 8;        // Sweep control pin

const int CS = 10;       // Chip select
//const int MOSI = 11;   // SPI MOSI
//const int MISO = 12;   // SPI MISO
//const int SCK = 13;    // SPI SCK

const int red_led = 3;   // RGB LED
const int green_led = 5;
const int blue_led = 6;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);

  delay(10);
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
  digitalWrite(P0, LOW);  // Ensure profile pin 0 low.
  digitalWrite(CS, LOW); // Ensure slave select h  igh
  
  
  dds_setup();
  //updateChannelFreqs(50000000);

  
   Serial.println("Startup Finished.");
}

void loop() {
  updateChannelFreqs(40000000);
  //delay (10000); //wait 10 seconds in case the constant loop is fucking it up somehow
}


// Set control registers. From AD9959 datasheet Pg.36 Table 28.
int dds_setup(){
  
  // Reset DDS
  ddsReset(); //at this point CSR should contain xF0

  // Channel Select Register: CSR (0x00)
  ddsWrite_8(CSR, 0x82); // Set Ch.3 EN, I/O mode 3-wire is 01 (10000010)b 
  
  // Function Register 1: FR1 (0x01)
  unsigned long FR1_val = 0x00A80000; // Sets the PLL to 10X.
  ddsWrite_24(FR1, FR1_val); // 1010 1000 0000 0000 0000 0000
  
  // Function Register 2: FR2 (0x02)
  // ddsWrite_16(FR2, 0x0000); // Default values.

  // Channel Function Register: CFR (0x03)
  // ddsWrite_24(CFR, 0x000302); // Default values.
  
  delay(10); // Wait for system to settle.
}


// Sets the frequency of a given channel.
void updateChannelFreqs(unsigned long RFfreq) { 
  //Serial.println("input RFfreq:");
  //Serial.println(RFfreq);
  //ddsWrite_8(CSR, 0x82); // Set Ch.3 EN, I/O mode 3-wire is 01 (1000 0010)
  //unsigned long ftwRF = MHzToFTW(RFfreq);
  unsigned long ftwRF = 171798691; //17179869; //  42949673 // FTW Max = 2147483648
  Serial.println("Checking that FTW conv. worked outside:");
  //unsigned long temp = FTWtoMHz(ftwRF);
  //Serial.println(temp);
  ddsWrite_32(CFTW, ftwRF);  //increment the RF channel FTW
  pulse(io_update);

  //ddsWrite_8(CSR, 0x02);  //close off channels
}

unsigned long MHzToFTW(unsigned long MHzFrequency) {         //given a freq, return a 32-bit FTW
  unsigned long FTWconst = (sys_clk / max32); //define freq <--> FTW conversion constant
  return (unsigned long) (MHzFrequency / FTWconst);   //return FTW
}

unsigned long FTWtoMHz(unsigned long FTW) {                 //given a 32-bit FTW, return a freq
  //Serial.println("FTW Inside helper:");
  //Serial.println(FTW);
  unsigned long FTWconst = (sys_clk / max32); //define freq <--> FTW conversion constant
  unsigned long temp = (unsigned long) (FTW * FTWconst);
  Serial.println("Checking conversion inside helper:");
  Serial.println(temp);
  return (unsigned long) (FTW * FTWconst);
}

//sweepDDS is basically the actual main() below:
/*
void sweepDDS(int punchIt)  {
  //first set starting frequency and other necessary things:
  unsigned long RFfreq = 40000000;
  unsigned long LOfreq = 39999000;
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
void ddsWrite_16(byte address, unsigned int val){
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(&val, 2);
  spiEnd();
}

// Simple function for writing 3 byte values (Untested)
void ddsWrite_24(byte address, unsigned long val){
  
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(&val, 3);
  spiEnd();

  /*
  byte a,b,c;
  a=(val      &0xFF); //extract first byte (LS)
  b=((val>>8) &0xFF); //extract second byte
  c=((val>>16)&0xFF); //extract third byte (MS)
  
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(a); // We want to transfer LSB first.
  SPI.transfer(b);
  SPI.transfer(c);
  spiEnd();
  */
}

// Simple function for writing 4 byte values
void ddsWrite_32(byte address, unsigned long val){
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(&val, 4);
  spiEnd();
}

void spiBegin(){
  SPI.beginTransaction(SPISettings(14000000, LSBFIRST, SPI_MODE0));
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
  digitalWrite(pin, LOW);
}

// Simple LED control function PWM.
void leds(int R, int G, int B) {
  analogWrite(red_led, 255-R);
  analogWrite(green_led, 255-G);
  analogWrite(blue_led, 255-B);
}

// Simple LED control function non-PWM.
void dleds(int R, int G, int B) {
  digitalWrite(red_led, 1-R);
  digitalWrite(green_led, 1-G);
  digitalWrite(blue_led, 1-B);
}
