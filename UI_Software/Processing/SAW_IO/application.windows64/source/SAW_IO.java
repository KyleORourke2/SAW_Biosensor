import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class SAW_IO extends PApplet {

// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


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

public void setup() {
  
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
public void draw() {
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
public void mousePressed() {
  header.click(mouseX, mouseY);
  toolbox.click(mouseX, mouseY);

}

// KEYBOARD INPUT DETECTED:
public void keyPressed() {
   
}

// HELPER FUNCTIONS:
public float getdB(String data){
  float parsed;
  String[] temp = split(data, ',');
  parsed = PApplet.parseFloat(temp[0]);
  //println("dB Parsed: " + parsed);
  return parsed;
}


public float getFrq(String data){
  float parsed;
  String[] temp = split(data, ',');
  parsed = PApplet.parseFloat(temp[1]);
  //println("Frq Parsed: " + parsed);
  return parsed;
}
// Button class:
// Places a button at the center of given coordinates 
// with a few nice features.

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


class Button {
  int fillColor; // Body color.
  int textColor; // Text color.
  int strokeColor; // Border color.
  String text; // Text in button centered.
  int shape; // Unused
  int Width;
  int Height;
  int xpos;
  int ypos;
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Button(int x, int y, int w, int h, int fC, int tC, int sC, String tS){
    fillColor = fC;
    textColor = tC;
    strokeColor = sC;
    text = tS;
    xpos = x;
    ypos = y;
    Width = w;
    Height = h;
  }
  
  // Display the button
  public void display(){
    // Bounding box
    stroke(strokeColor);
    strokeWeight(1);
    if(hovering(mouseX, mouseY)){
      fill(hoverFade);
    } else {
      fill(fillColor);
    }
    rectMode(CENTER);
    rect(xpos, ypos, Width, Height);
    
    // Inside text
    fill(textColor);
    textSize(25);
    textAlign(CENTER, CENTER);
    text(text, xpos, ypos);
  }
  
  // Check if a click is in the button box.
  public boolean wasClicked(int mx, int my){
    if(mx >= xpos - Width/2 && mx <= xpos + Width/2){
      if(my >= ypos - Height/2 && my <= ypos + Height/2){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Checks if mouse is over the button.
  public boolean hovering(int mx, int my){
    if(mx >= xpos - Width/2 && mx <= xpos + Width/2){
      if(my >= ypos - Height/2 && my <= ypos + Height/2){
        return true;
      }
    }
    return false;
  }
  
} // Button class end.
// Graph class:
// Takes in data object or function and plots it on
// an X Y plot.


class Graph {
  
  int lineColor;
  int graphColor;
  String xText; // x Axis label
  String yText; // y Axis label
  String tText; // Title text
  int xpos;     // x Location of graph
  int ypos;     // y Location of graph
  int Width;    // x Size of graph
  int Height;   // y Size of graph
  float[] DataX;
  float[] DataXS; // Scaled x axis data for visualization.
  float[] DataY;
  float[] DataYS;
  
  int fmin = 40000000;
  int fmax = 60000000;
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Graph(float[] dataX, float[] dataY, int x, int y, int w, int h, int lineC, String xT, String yT, String tT){
    lineColor = lineC;
    xText = xT;
    yText = yT;
    tText = tT;
    xpos = x;
    ypos = y;
    Width = w;
    Height = h;
    DataX = dataX.clone(); // Copies of original data to be used in helper functions.
    DataY = dataY.clone();
    DataXS = dataX.clone(); // Scaled versions of the data.
    DataYS = dataY.clone();
  }
  
  public void display(){
       
    // Draw graph box:
    drawBox(xpos, ypos, Width, Height);
    drawGrid(xpos, ypos, Width, Height);
    
    if(DataX.length > 1 && DataY.length > 1){
      drawTickVals(DataX.clone(), xpos, ypos, Width, false); // draw horizontal ticks
      drawTickVals(DataY.clone(), xpos, ypos, Height, true); // draw horizontal ticks
      
      // Scale data to fill entire length of graph box
      DataXS = arrayScale_X(DataX.clone(), xpos, Width, fmin, fmax);
      DataYS = arrayScale_Y(DataY.clone(), ypos, Height);
      // Adjust array by xpos & ypos
      //DataXS = arrayShift(DataXS.clone(), xpos);
      DataYS = arrayShift(DataYS.clone(), ypos);
      // Set Y values to correct coordinate system.
      DataYS = arrayFlip(DataYS.clone(), ypos);
      
      // Draw graph line
      for(int i = 0; i < DataXS.length-1; i++){
        strokeWeight(1);
        stroke(lineColor);
        line(DataXS[i], DataYS[i], DataXS[i+1], DataYS[i+1]);
        println("X: " + DataXS[i] + " Y: " + DataYS[i]);
      }
    }
  } // Display end
  
  
  //////////////// HELPER FUNCTIONS ////////////////
  public void updateDataSet(float[] dataX, float[] dataY){
    DataX = dataX.clone(); // Copies of original data to be used in helper functions.
    DataY = dataY.clone();
    DataXS = dataX.clone(); // Scaled versions of the data.
    DataYS = dataY.clone();
  }
  
  
  // Check if the was clicked location has an action
  public void click(int mx, int my){

  }
  
  // Check if a click is in an action area.
  public boolean wasClicked(int mx, int my){
    if(mx >= xpos && mx <= xpos + Width){
      if(my >= ypos && my <= ypos + Height){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Draw text
  private void drawLabels(int x, int y, int w, int h){
    fill(darkGrey);
    textAlign(CENTER, CENTER);
    textSize(30);
    text(tText, x+(w/2), y-h+25);
    textSize(20);
    text(xText, x+(w/2), y+35);
    pushMatrix();
    translate(x,y);
    rotate(-HALF_PI);
    text(yText, h/2, -35);
    popMatrix(); 
  }
  
  // Draw tick values
  // data: Array of data to print on given axis.
  // x: starting location on horizontal axis.
  // y: starting location on vertical axis.
  // size: width/height of graph.
  // vert: sets printing direction to either horizonal or vertical.
  private void drawTickVals(float[] data, int x, int y, int size, boolean vert){
    fill(ucscYellow);            // Set text color
    textAlign(CENTER, CENTER);   // Align center
    textSize(20);                // Set size
    
    // Print tick marks based on orientation:
    
    if(vert == false){
      float maxVal = 60000000;//data[data.length-1]; // /1000000; // Add if data actually is in MHz.
      float minVal = 40000000;
      float span = maxVal - minVal;
      for(int i=1; i < 10; i++){
        text(PApplet.parseInt(minVal+(span/10)*i), x+(i*(size/10)), y-15);
      }
      textAlign(RIGHT, CENTER);
      text("Hz", x+(10*(size/10))-2, y-15);
      textAlign(CENTER, CENTER);
    }
    else{
      textAlign(LEFT, CENTER);
      float maxVal = data[findMax(data)];
      float minVal = data[findMin(data)];
      float span = maxVal - minVal;
      text(PApplet.parseInt(maxVal)+" dB", x+5, y-Height+10);
      for(int i=1; i < 10; i++){
        text(PApplet.parseInt(minVal+(span/10)*i), x+5, y-(i*(size/10))-2);
      }
      text(PApplet.parseInt(minVal), x+5, y-20);
      textAlign(CENTER, CENTER);
    }

  }
  
  // Draw Grid
  private void drawGrid(int x, int y, int w, int h){
    strokeWeight(0);
    stroke(lightGrey);
    // Vertical
    for(int i=0; i < 10; i++){
      line(x+(i*(w/10)), y, x+(i*(w/10)), y-h);
    }
    // Horizontal
    for(int i=0; i < 10; i++){
      line(x, y-(i*(h/10)), x+w, y-(i*(h/10)));
    }
  }
  
  // Draw graph box
  private void drawBox(int x, int y, int w, int h){
    strokeWeight(3);
    stroke(darkGrey);
    line(x, y, x+w, y); // Bottom line
    line(x, y, x, y-h); // Left line
    line(x+w, y, x+w, y-h); // Right line
    line(x, y-h, x+w, y-h); // Right line
    fill(10,10,10);
    rectMode(CENTER);
    rect(x+(w/2), y-(h/2), w, h);
  }
  
  // Scales data to fit graph box
  private float [] arrayScale_X(float[] data, int xpos, int Width, int fmin, int fmax){
    for(int i = 0; i < data.length; i++){
      data[i] = map(data[i], fmin, fmax, xpos, (xpos+Width));
    }
    return data;
  }
  
  // Scales data to fit graph box vertically
  private float [] arrayScale_Y(float[] data, int ypos, int h){
    
    float maxVal = data[findMax(data)];
    float minVal = data[findMin(data)];
    float valRange = abs(maxVal-minVal);
    
    float y_scaleFactor = (h/valRange);
    data = arrayMult(data, y_scaleFactor);
    
    // Now shift the graph into frame.
    if(data[findMax(data)] > h){
      data = arrayShift(data, -(data[findMax(data)]-h));
    }
    if(data[findMin(data)] < 0){
      data = arrayShift(data, 0-(data[findMin(data)]));
    }
    
    return data;
  }
  
  
  // Find max value location. (-1 if failed)
  private int findMax(float[] data){
    int loc = 0;
    float currentMax = data[0];
    for (int i = 0; i < data.length; i++){
      if(currentMax < data[i]){
        currentMax = data[i];
        loc = i;
      }
    }
    return loc;
  }
  
  // Find min value location. (-1 if failed)
  private int findMin(float[] data){
    int loc = 0;
    float currentMin = data[0];
    for (int i = 0; i < data.length; i++){
      if(currentMin > data[i]){
        currentMin = data[i];
        loc = i;
      }
    }
    return loc;
  }
  
  // Shifts all of array's values by set amount.
  private float [] arrayShift(float[] data, float shift){
    for (int i = 0; i < data.length; i++){
      data[i] += shift;
    }
    return data;
  }
  
  // Multiplies data by given value.
  private float [] arrayMult(float[] data, float x){
    for (int i = 0; i < data.length; i++){
      data[i] = x*data[i];
    }
    return data;
  }
  
  // Flips data along given value.
  private float [] arrayFlip(float[] data, float x){
    for (int i = 0; i < data.length; i++){
      data[i] = data[i] - 2*(data[i] - x);
    }
    return data;
  }
  
  // Maps a value to a defined range.
  public float map(float x, int in_min, int in_max, int out_min, int out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
}



 
// Header class:
// Provides a way to populate the top header bar easily

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


class Header {
  int xpos = 0;
  int ypos = 0;
  int Height = headerHeight; // Thickness of the header.
  int Width = width;   // should span entire width.
  int fillColor;
  
  Button exitButton; // Button to close program.
  
  Header(){
    exitButton = new Button(width-25, headerHeight/2, 50, headerHeight-1, exitFill, exitText, exitStroke, "X");
  }
  
  public void display(){
    // Header bar:
    noStroke();
    fill(headerFill);
    rect(0, 0, Width, Height);
    
    // Header Center Text
    fill(ucscYellow);
    textSize(30);
    textAlign(CENTER, CENTER);
    text("SlugScope", Width/2, Height/2);
    
    // Objects in header:
    exitButton.display();
  }

  // Check if a click is in the header box.
  public boolean wasClicked(int mx, int my){
    if(mx >= xpos && mx <= xpos + Width){
      if(my >= ypos && my <= ypos + Height){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Check if the was clicked location has an action
  private void click(int mx, int my){
    if(exitButton.wasClicked(mx, my)){ // Exit button action.
      exit();
    }
  }
  
} // Header class end.
// Serial Monitor class:
// Location of serial I/O readout. Should be at bottom of the screen

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 5/27/2019


class SerialMonitor {
  int xpos;
  int ypos;
  int Height = SerialMonitorHeight; // Thickness of the header.
  int Width = width-1; // should span entire width.
  String[] serialBuffer = {}; // Need to init one value for some reason...
  
  SerialMonitor(int x, int y){
    xpos = x;
    ypos = y;
  }
  
  public void display(String newSerial){
    
    // Bounding box:
    stroke(SerialMonitorBorder);
    strokeWeight(2);
    fill(SerialMonitorFill);
    rect(xpos, ypos, Width, Height);
    fill(SerialMonitorBorder);
    rect(xpos, ypos - (Height/2)+10, Width, 30);

    // Upper Information text bar:
    fill(255);
    textSize(20);
    textAlign(RIGHT, CENTER);
    text(serialPort, xpos + (Width/2)-5, ypos - (Height/2)+8);
    textAlign(LEFT, CENTER);
    text("Serial Monitor", xpos - (Width/2)+5, ypos - (Height/2)+8);    
    
    
    // EVERYTHING TO DO WITH 'FAKE' SERIAL PORT:
    
    // Get new value and add to buffer array:
    if(newSerial != null){
      serialBuffer = reverse(serialBuffer); // Flips the data since append only puts it on the end.
      serialBuffer = append(serialBuffer, newSerial);
      serialBuffer = reverse(serialBuffer);
    }
    
    // Print the values in our serial terminal box in ascending order.
    for(int i = 0; i < serialBuffer.length && i < 8; i++){
      text(serialBuffer[i], xpos - (Width/2)+5, ypos + ((Height/2)-17) - 21*i);
    }
    
    // Delete values from serialBuffer to keep memory usage low:
    if(serialBuffer.length >= 9){
      serialBuffer = shorten(serialBuffer);
    }
    
    textAlign(CENTER, CENTER);
  }

  // Serial port grab incomming data:
  // THIS FUNCTION IS BAD SINCE IT CAN MISS INCOMMING DATA! (I think?)
  // However, It is not used in the graphing function and only really is for
  // displaying that data is comming in. :D
  public String getSerial(){
    if(bioPort.available() > 0){
      String inBuffer = bioPort.readStringUntil('\n');  // Read until newline.
      if(inBuffer != null){
        return inBuffer;
      }
      else{
        return null;
      }
    }
    else{
      return null;
    }
  }


  // Check if a click is in the header box.
  public boolean wasClicked(int mx, int my){
    if(mx >= xpos && mx <= xpos + Width){
      if(my >= ypos && my <= ypos + Height){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Check if the was clicked location has an action
  private void click(int mx, int my){

  }
  
}
// Textbox class:
// Places a texbox at the center of given coordinates 
// and manages value inputs

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019
// Color theme file.

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019

// Sets the color and sizes of certain elements in the program.

// UCSC Colors
int ucscYellow = 0xfffdc700;
int ucscBlue = 0xff003c6c;

// Defaults:
int lightGrey = color(245,245,246);
int darkGrey = color(129,139,149);

// Header
int headerHeight = 50;
int headerWidth = width;
int headerFill = ucscBlue; //#0077c2;
int subheaderFill = 0xff42a5f5;

// Toolbox
int toolbarFill = 0xff42a5f5;
int toolboxHeight = 150;
int toolboxWidth = 200;

// Serial Monitor
int SerialMonitorHeight = 200;
int SerialMonitorWidth = width;
int SerialMonitorTxt = color(255, 255, 255); // White text.
int SerialMonitorFill = 0; // Black background.
int SerialMonitorBorder = darkGrey;

// Buttons
int hoverFade = 0xff757575;

// EXIT (Top right corner):
int exitFill = 0xffff3d00;
int exitStroke = ucscBlue;//exitFill;//#0077c2; //#fafafa;
int exitText = 0xfffafafa;

// Graph colors
int lineColor = 0xff000000;
int graphColor = 0;
int gridLine = color(0, 100, 0);
int greenText = color(141, 244, 66);  // Color of text.
int ch1_color = color(255, 160, 20);  // Color of channel 1 data.
int ch2_color = color(20, 160, 255);
int ch3_color = color(141, 244, 66);
// Toolbox class:
// Location for clickable tools and things like that?

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 5/25/2019


class Toolbox {
  int xpos;
  int ypos;
  int Height = toolboxHeight; // Thickness of the header.
  int Width = toolboxWidth; // should span entire width.
  
  Button sweepRun; // Button to close program.
  Button setFreq;
  
  Toolbox(int x, int y){
    xpos = x;
    ypos = y;
    
    // BUTTONS:
    sweepRun = new Button(xpos, ypos+(Height/2)-25, Width-1, 50, ucscYellow, graphColor, graphColor, "Sweep");
    setFreq  = new Button(xpos, ypos+(Height/2)-75, Width-1, 50, ucscYellow, graphColor, graphColor, "Test Frequency");
  }
  
  public void display(){
    // Bounding box:
    noStroke();
    fill(toolbarFill);
    rect(xpos, ypos, Width, Height);
    
    // Toolbox Top Text
    fill(0);
    textSize(30);
    textAlign(CENTER, CENTER);
    text("Toolbox", xpos, ypos - (Height/2)+20);
    
    // Objects in header:
    sweepRun.display();
    setFreq.display();
  }
  
  // Check if a click is in the header box.
  public boolean wasClicked(int mx, int my){
    if(mx >= xpos && mx <= xpos + Width){
      if(my >= ypos && my <= ypos + Height){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Check if the was clicked location has an action
  private void click(int mx, int my){
    if(sweepRun.wasClicked(mx, my)){ // Sweep button action.
      bioPort.write("S 40000000 60000000 10000"); 
    }
    if(setFreq.wasClicked(mx, my)){ // Sweep button action.
      bioPort.write("F 10000000"); 
    }
  }
  
}
  public void settings() {  fullScreen(); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "SAW_IO" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
