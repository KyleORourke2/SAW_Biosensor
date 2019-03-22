import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

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


Header header;
PImage img;


public void setup() {
  
  background(lightGrey);
  header = new Header();
  img = loadImage("data.png");
}

public void draw() {
  frameRate(35);
  header.display();   // draw header bar
  image(img, 350, 200, 1800, 1100);
}


public void mousePressed() {
  header.click(mouseX, mouseY);
  
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
  String xText;
  String yText;
  int xpos;
  int ypos;
  int Width;
  int Height;
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Graph(int data[], int x, int y, int w, int h, int lineC, String xT, String yT){
    lineColor = lineC;
    xText = xT;
    yText = yT;
    xpos = x;
    ypos = y;
    Width = w;
    Height = h;
  }
  
  public void display(){
    
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
  int Width = width; // should span entire width.
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
  public void click(int mx, int my){
    if(wasClicked(mx, my)){ // The header area was clicked.
      if(exitButton.wasClicked(mx, my)){ // Exit button action.
        exit();
      }
    }
  }
  
} // Header class end.
// Color theme file.

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019

// Sets the color and sizes of certain elements in the program.

// UCSC Colors
int ucscYellow = 0xfffdc700;
int ucscBlue = 0xff003c6c;

// Header
int headerHeight = 50;
int headerFill = ucscBlue; //#0077c2;
int subheaderFill = 0xff42a5f5;

// Toolbar
int toolbarFill = 0xff42a5f5;

// Buttons
int hoverFade = 0xff757575;

// EXIT (Top right corner):
int exitFill = 0xffff3d00;
int exitStroke = 0xff0077c2; //#fafafa;
int exitText = 0xfffafafa;


// Background
int lightGrey = color(245,245,246);
// Toolbox class:
// Location for clickable tools and things like that?

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/10/2019



class Toolbox {
  
  
  
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
