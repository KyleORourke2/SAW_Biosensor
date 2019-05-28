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
  
  void display(){
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
  boolean wasClicked(int mx, int my){
    if(mx >= xpos && mx <= xpos + Width){
      if(my >= ypos && my <= ypos + Height){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Check if the was clicked location has an action
  void click(int mx, int my){
    if(sweepRun.wasClicked(mx, my)){ // Sweep button action.
      bioPort.write("S 40000000 60000000 10000"); 
    }
    if(setFreq.wasClicked(mx, my)){ // Sweep button action.
      bioPort.write("F 10000000"); 
    }
  }
  
}
