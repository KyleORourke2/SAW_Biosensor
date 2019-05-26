// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019

import processing.serial.*;
Serial bioPort;

Header header;
Toolbox toolbox;
Graph graph1, graph2, graph3;

PImage img;
float[] dataY1 = {-10,10,24,31,10,50,4.6,70,100,45,0}; // Example data
float[] dataY2 = {100,10,204,301,100,50,46,70,140,45,200};
float[] dataY3 = {600,500,300,100,10,50,10,70,100,45,0};
float[] dataX =  {0,10,20,30,40,50,60,70,80,90,100};

void setup() {
  bioPort = new Serial(this, "COM19", 9600);
  
  fullScreen();
  background(lightGrey);
  header = new Header();
  toolbox = new Toolbox(110, (toolboxHeight/2)+headerHeight+10);
  //graph1 = new Graph (dataX, dataY1, 300, 800, 600, 400, ch1_color, "Frequency", "Y-Value", "Example Graph 1");
  //graph2 = new Graph (dataX, dataY2, 1000, 800, 600, 400, ch2_color, "Frequency", "Y-Value", "Example Graph 2");
  //graph3 = new Graph (dataX, dataY3, 1700, 800, 600, 400, ch3_color, "Frequency", "Y-Value", "Example Graph 2");
  
  
}

void draw() {
  frameRate(16);
  header.display();   // draw header bar
  toolbox.display();

  while (bioPort.available() > 0) {
    String inBuffer = bioPort.readString();   
    if (inBuffer != null) {
      println(inBuffer);
    }
  }
  
}


void mousePressed() {
  header.click(mouseX, mouseY);
  toolbox.click(mouseX, mouseY);
  //graph1.click(mouseX, mouseY);
  //graph2.click(mouseX, mouseY);
  //graph3.click(mouseX, mouseY);

}



void keyPressed() {
   
}
