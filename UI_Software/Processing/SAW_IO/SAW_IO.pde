// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019

import processing.serial.*;
Serial bioPort;
String serialPort = "COM19";

Header header;
Toolbox toolbox;
SerialMonitor serialMonitor;
Graph graphSerial, graph2, graph3;

float[] dataY1 = {-4,-5,-9,-25,-45,-50,-66,-70,-64,-45,-1}; // Example data
float[] dataX =  {0,10,20,30,40,50,60,70,80,90,100};   // Example data

float[] sDataY = {}; // Serial data holder.
float[] sDataX = {}; // Serial data holder.


void setup() {
  fullScreen();
  background(lightGrey);
  header = new Header();
  toolbox = new Toolbox(100, (toolboxHeight/2)+headerHeight);
  serialMonitor = new SerialMonitor(width/2, height - (SerialMonitorHeight/2)-1);
  graphSerial = new Graph (dataX, 
                           dataY1, 
                           100*2+1, 
                           (height-SerialMonitorHeight-8), 
                           (width-(100*2)-3), 
                           (height-SerialMonitorHeight-headerHeight-9), 
                           ucscYellow, 
                           "Frequency (MHz)", "-dB", "S21");
  bioPort = new Serial(this, serialPort, 9600);
  bioPort.clear();
}

// DRAW ALL OBJECTS
void draw() {
  frameRate(16);
  header.display();
  toolbox.display();
  serialMonitor.display();
  graphSerial.display();
  
  
 
}

// MOUSE CLICK DETECTED:
void mousePressed() {
  header.click(mouseX, mouseY);
  toolbox.click(mouseX, mouseY);

}

// KEYBOARD INPUT DETECTED:
void keyPressed() {
   
}
