// biosaw_lite.ino
// Biolite Firmware

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

// DDS VALUES:
#define ref_clk 25000000          // 25Mhz external reference clock.
#define sys_clk 250000000         // 250Mhz internal frequency from PLL.
#define FTW_CFB 0.05820766091     // Constant used for finding FTW: 250MHz/(2^32)
#define FTW_CBF 17.179869184      // Constant used for finding FTW: (2^32)/250MHz

// DDS REGISTERS:
const byte CSR  = 0x00; // Channel Select Register
const byte FR1  = 0x01; // Function Register 1: VCO, PLL
const byte FR2  = 0x02; // Function Register
const byte CFR  = 0x03; // Channel Function Register
const byte CFTW = 0x04; // Channel Frequency Tunning Word

// DDS REGISTER VALUES:
// Sets the value to be sent to each register.
// FIXED VALUES:
const byte          CSR_V = 0x82;       // Set Ch.3 EN, I/O mode 3-wire is 01 (1000 0010)
const unsigned long FR1_V = 0x00A80000; // Set VCO ON and PLL to 10x. 0010 1000
const unsigned long FR2_V = 0x00;       // Unused.
const unsigned long CFR_V = 0x00000302; // Set DAC FS current, Clear Phase accumulator.
// VARIABLE VALUES:
unsigned long CFTW_V = 0;  // Frequency tuning word. 32-bits.

// PIN DEFINITIONS:
const int rst_dds = 2;   // DDS reset pin.
const int io_update = 4; // DDS I/O update toggle.
const int P0 = 8;        // Sweep control pin
const int CS = 10;       // Chip select
//const int MOSI = 11;   // SPI MOSI
//const int MISO = 12;   // SPI MISO
//const int SCK = 13;    // SPI SCK
const int red_led = 3;   // RGB LED
const int green_led = 5; // RGB LED
const int blue_led = 6;  // RGB LED

// SPI SETTINGS:
SPISettings settings(20000000, MSBFIRST, SPI_MODE0); 

// SETUP:
void setup() {

  // SERIAL SETUP:
  Serial.begin(115200); // USER'S COMPUTER SERIAL COM SETUP (NOT SPI)
  Serial.print("BioLite Startup...\n\n");
  Serial.print("FTW_CFB = "); Serial.print(FTW_CFB, 9);
  Serial.print("  Should be: "); Serial.println("0.05820766091");
  Serial.print("FTW_CBF = "); Serial.print(FTW_CBF, 8);
  Serial.print("  Should be: "); Serial.println("17.179869184\n");
  delay(10);
  

  // PINMODE DEFINITIONS:
  pinMode(CS,        OUTPUT);
  pinMode(rst_dds,   OUTPUT);
  pinMode(io_update, OUTPUT);
  pinMode(P0,        OUTPUT);
  pinMode(red_led,   OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led,  OUTPUT);

  // SET PIN INITIAL VALUES:
  dleds(0,0,0);
  digitalWrite(P0, LOW); // Ensure profile pin 0 low.
  digitalWrite(CS, LOW); // Ensure slave select high to start.
  
  // SPI STARTUP MUST OCCUR AFTER PIN DEFINITIONS & BEFORE DDS SETUP:
  SPI.begin();

  // DDS SET INITIAL REGISTER VALUES:
  //dds_setup();

  // SETUP FINISHED:
  Serial.println("Startup Finished.");
}

// MAIN LOOP:
void loop() {
  setFrequency(25000000); // Set the output of the DDS to 50MHz.
  delay(10000); // Small delay in main loop to prevent instability.
}


// SETS THE OUTPUT OF THE DDS TO THE GIVEN FREQUENCY
void setFrequency(float frequency){
  CFTW_V = calc_FTW(frequency);
  ddsReset();
  ddsWrite_8(CSR, CSR_V); // Enable Ch3 & SPI 3-wire
  
  ddsWrite_24(FR1, FR1_V); // PLL & VCO

  ddsWrite_32(CFTW, CFTW_V); // FTW
  pulse(io_update);
}


// Calculates the needed FTW for a desired frequency output:
unsigned long calc_FTW(float frequency){
  // Formula for finding frequency f_out = (FTW*250MHz)/(2^32)
  // Changes to FTW = f_out * FTW_CBF, where FTW_CBF is precalculated 2^32/250MHz.
  return (frequency * FTW_CBF);
}

// DDS SETUP:
// Reset DDS and set initial control registers. 
// More information on AD9959 datasheet Pg.36 Table 28.
int dds_setup(){
  
  // Toggle DDS Reset Pin:
  ddsReset(); // AFTER RESET CSR SHOULD CONTAIN 0XF0

  // Channel Select Register: CSR (ADR: 0x00)
  ddsWrite_8(CSR, CSR_V);
  
  // Function Register 1: FR1 (ADR: 0x01)
  ddsWrite_24(FR1, FR1_V);

  // Channel Function Register: CFR (ADR: 0x03)
  ddsWrite_24(CFR, CFR_V);

  delay(10); // Wait for system to settle.
}

// MASTER DDS RESET:
void ddsReset(){
  pulse(rst_dds); // Pulse reset pin.
}

// Simple function for writing 8 bit values
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

  byte a,b,c;
  a = (val       & 0xFF); //extract first byte (LS)
  b = ((val>>8)  & 0xFF); //extract second byte
  c = ((val>>16) & 0xFF); //extract third byte (MS)
  
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(c); // We want to transfer MSB first.
  SPI.transfer(b);
  SPI.transfer(a);
  spiEnd();
}

// Simple function for writing 4 byte values
void ddsWrite_32(byte address, unsigned long val){

  byte a,b,c,d;
  a = (val       & 0xFF); //extract first byte (LS)
  b = ((val>>8)  & 0xFF); //extract second byte
  c = ((val>>16) & 0xFF); //extract third byte
  d = ((val>>24) & 0xFF); //extract fourth byte
  
  spiBegin();
  SPI.transfer(address);
  SPI.transfer(d);  // We want to transfer MSB first.
  SPI.transfer(c);
  SPI.transfer(b);
  SPI.transfer(a);
  spiEnd();
}

void spiBegin(){
  SPI.beginTransaction(settings); // Define SPI settings before transfer.
  digitalWrite(CS, LOW);          // Enable slave select.
}

void spiEnd(){
  digitalWrite(CS, HIGH);  // Disable slave select.
  SPI.endTransaction();    // End SPI transfer.
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
