// biosaw_lite.ino
// Biolite Firmware

// Written by: Kyle O'Rourke & Jackson Atwater
// 5/14/2019
// UCSC SDP

#include <SPI.h>

// DDS VALUES:
#define ref_clk 25000000          // 25Mhz external reference clock.
#define sys_clk 500000000         // 500Mhz internal frequency from PLL.
#define FTW_CFB 0.11641532182     // Constant used for finding FTW: 500MHz/(2^32)
#define FTW_CBF 8.589934592       // Constant used for finding FTW: (2^32)/500MHz

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
const unsigned long FR1_V = 0x00D00000; // Set VCO ON and PLL to 20x. 1101 0000
const unsigned long FR2_V = 0x00;       // Unused.
const unsigned long CFR_V = 0x00000302; // Set DAC FS current, Clear Phase accumulator.
// VARIABLE VALUES:
unsigned long CFTW_V = 0;  // Frequency tuning word. 32-bits.

// ADC VARIABLES:
#define pwrSlope 0.0199 // mV/dB 
#define ext_vref 1.939  // Voltage on external voltage ref.
const float voltMul = (ext_vref)/(1024); // Value of (ext_vref)/(2^10) Shoul be: 0.0018935546875

// PIN DEFINITIONS:1
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
const int ADC_1 = A0;    // ADC A0 Ref
const int ADC_2 = A2;    // ADC A2 Test

// SPI SETTINGS:
SPISettings settings(20000000, MSBFIRST, SPI_MODE0); 

// SETUP:
void setup() {

  // SERIAL SETUP:
  Serial.begin(115200); // USER'S COMPUTER SERIAL COM SETUP (NOT SPI)
  Serial.print("BioLite Startup...\n");
  Serial.print("voltMul = "); Serial.print(voltMul, 9);
  Serial.print("  Should be: "); Serial.print("0.0018935546875\n");
  Serial.print("FTW_CFB = "); Serial.print(FTW_CFB, 9);
  Serial.print("  Should be: "); Serial.print("0.11641532182\n");
  Serial.print("FTW_CBF = "); Serial.print(FTW_CBF, 8);
  Serial.print("  Should be: "); Serial.print("8.589934592\n");
  delay(10);
  

  // PINMODE DEFINITIONS:
  pinMode(CS,        OUTPUT);
  pinMode(rst_dds,   OUTPUT);
  pinMode(io_update, OUTPUT);
  pinMode(P0,        OUTPUT);
  pinMode(red_led,   OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led,  OUTPUT);
  pinMode(ADC_1,      INPUT);
  pinMode(ADC_2,      INPUT);

  // SET PIN INITIAL VALUES:
  digitalWrite(P0, LOW); // Ensure profile pin 0 low.
  digitalWrite(CS, LOW); // Ensure slave select high to start.
  startupLEDS();         // Little LED show for startup.
  
  // SPI STARTUP MUST OCCUR AFTER PIN DEFINITIONS & BEFORE DDS SETUP:
  SPI.begin();

  // DDS SET INITIAL REGISTER VALUES:
  dds_setup();

  // ADC REFERENCE SETUP:
  analogReference(EXTERNAL);

  // SETUP FINISHED:
  Serial.println("Startup Finished.");
}

// MAIN LOOP:
void loop() {
  //leds(100,0,0);
  setFreqMhz(10);
  for(unsigned int i = 40000; i <= 60000; i = i+10){
    setFreqkHz(i);
    delay(10);
    int adc1_in = analogRead(ADC_1); // ref
    int adc2_in = analogRead(ADC_2); // test
    //Serial.print("ADC: "); Serial.print(adc1_in); Serial.print("  ADC2: "); Serial.println(adc2_in);
    int adc_dif = adc2_in - adc1_in;
    float dif_vol = convVol(adc_dif);
    float dB_in   = convPwr(dif_vol);
    //Serial.print("dB: "); Serial.print(dB_in, 5); Serial.print("  Freq (Mhz): "); Serial.println(i);
    Serial.print(dB_in, 5); Serial.print(", "); Serial.println(i);
  }
  Serial.println("SWEEP END");
  leds(0, 100, 50);
  end();
}

// CONVERT VOLTAGE READING TO OUTPUT POWER:
float convPwr(float voltage){
  return (voltage/pwrSlope);
}

// CONVERT ADC VALUE TO VOLTAGE:
// RETURNS VOLTAGE AS A FLOAT
float convVol(int adc_val){
  return float(adc_val*voltMul);
}

// SETS THE OUTPUT OF THE DDS TO THE GIVEN FREQUENCY
void setFrequency(float frequency){
  if(frequency < 40000000){
    dleds(1,0,0); // Current power detectors only go down to 40Mhz.
  }
  else{
    dleds(0,0,0);
  }
  CFTW_V = calc_FTW(frequency);
  ddsWrite_32(CFTW, CFTW_V); // FTW
  pulse(io_update);
}

void setFreqMhz(float frequency){
  setFrequency(frequency*1000000);
}

void setFreqkHz(float frequency){
  setFrequency(frequency*1000);
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
void dds_setup(){
  
  // Toggle DDS Reset Pin:
  ddsReset(); // AFTER RESET CSR SHOULD CONTAIN 0XF0

  // Channel Select Register: CSR (ADR: 0x00)
  ddsWrite_8(CSR, CSR_V);
  
  // Function Register 1: FR1 (ADR: 0x01)
  ddsWrite_24(FR1, FR1_V);

  // Channel Function Register: CFR (ADR: 0x03)
  //ddsWrite_24(CFR, CFR_V);

  pulse(io_update); // Make settings take effect.
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

// Cute little LED start function:
void startupLEDS(){
  for(int i = 0; i < 255; i++){
    leds(0,0,i);
    delay(3);
  }
  for(int i = 0; i < 255; i++){
    leds(0,i,255-i);
    delay(2);
  }
  for(int i = 255; i > 0; i--){
    leds(0,i,0);
    delay(2);
  }
  dleds(0,0,0);
}

//Looks for incoming serial information and if found, goes into diagnostics loop. 
/*
void serialControl(){
	if(Serial.available() > 0){		//Checks for serial data.
		while(true){
			if(Serial.available() > 0){
				//Read the incoming.
        String incoming = Serial.readString();
        //Echo incoming.
        Serial.println();
        Serial.print("Received: ");
        Serial.println(incoming);
        incoming.toLowerCase();
        //Actions:
        if(incoming == "diagnostics" || incoming == "info"){
          print_diagnostics();
        }
        else if(incoming == "servo test"){
          leds_off();
          servo.attach(servo_pin);
					green(0);
					servo.write(settings.servo_release_pos);
					delay(1000);

					green(150);
					red(0);
					servo.write(settings.servo_mid_pos);
					delay(1000);

					green(255);
					servo.write(settings.servo_lock_pos);
					delay(1000);

					leds_off();
					servo.detach();
        }
        else if(incoming == "exit"){
          leds_off();
          break;
        }
        else if(incoming == "help"){
          Serial.println();
          Serial.println("Valid commands:");
          Serial.println("1) diagnostics or info");
          Serial.println("2) arm");
          Serial.println("3) servo test");
          Serial.println("4) exit");
          Serial.println("(Not Case sensitive.)");
        }
        else{
          Serial.println();
          Serial.println("Command not recognized! Type 'help' for more info.");
        }
	    }

	    //Fade blue while in Serial loop.
      for(int i = 0; i != 255; i++){
				leds(0,0,i);
				delay(4);
			}
			for(int i = 255; i != 0; i--){
				leds(0,0,i);
				delay(4);
			}
		}
	}
}
*/

float readVcc(){
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  float result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}