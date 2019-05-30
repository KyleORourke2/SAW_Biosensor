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
boolean sweepSerial = false; // Flag if current serial data is sweep data.

void setup() {
  fullScreen();
  background(lightGrey);
  header = new Header();
  toolbox = new Toolbox(100, (toolboxHeight/2)+headerHeight);
  serialMonitor = new SerialMonitor(width/2, height - (SerialMonitorHeight/2)-1);
  graphSerial = new Graph (sDataX, 
                           sDataY, 
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
  header.display();
  toolbox.display();
  graphSerial.updateDataSet(sDataX, sDataY);
  graphSerial.display();
  
  String serialBuffer = trim(serialMonitor.getSerial()); // Grabs next line in serial buffer.
  serialMonitor.display(serialBuffer);
  
  // ANYTHING dependent on new serial data needs to go here:
  if(serialBuffer != null){
    //println(serialBuffer);
    
    // This needs to happen before the code below... Cause it's late and I'm getting lazy...
    if(sweepSerial == true && !serialBuffer.equals("SWEEP END")){  // If new sweep data, fill buffer.
      sDataY = append(sDataY, getdB(serialBuffer));
      sDataX = append(sDataX, getFrq(serialBuffer));
    }
    if(serialBuffer.equals("SWEEP START")){  // Check for new sweep data. Flawed if device fails.
      sweepSerial = true;
    }
    if(serialBuffer.equals("SWEEP END")){ // Flawed if device malfunctions and doesn't output the string.
      sweepSerial = false;
    }
  } // if END
} // draw END

// MOUSE CLICK DETECTED:
void mousePressed() {
  header.click(mouseX, mouseY);
  toolbox.click(mouseX, mouseY);

}

// KEYBOARD INPUT DETECTED:
void keyPressed() {
   
}

// HELPER FUNCTIONS:
float getdB(String data){
  float parsed;
  String[] temp = split(data, ',');
  parsed = float(temp[0]);
  //println("dB Parsed: " + parsed);
  return parsed;
}


float getFrq(String data){
  float parsed;
  String[] temp = split(data, ',');
  parsed = float(temp[1]);
  //println("Frq Parsed: " + parsed);
  return parsed;
}
