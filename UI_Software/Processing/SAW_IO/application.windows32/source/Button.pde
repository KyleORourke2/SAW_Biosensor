// Button class:
// Places a button at the center of given coordinates 
// with a few nice features.

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


class Button {
  color fillColor; // Body color.
  color textColor; // Text color.
  color strokeColor; // Border color.
  String text; // Text in button centered.
  int shape; // Unused
  int Width;
  int Height;
  int xpos;
  int ypos;
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Button(int x, int y, int w, int h, color fC, color tC, color sC, String tS){
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
  boolean wasClicked(int mx, int my){
    if(mx >= xpos - Width/2 && mx <= xpos + Width/2){
      if(my >= ypos - Height/2 && my <= ypos + Height/2){
        return true;
      }
    }
    return false; // Didn't meet both conditions
  }
  
  // Checks if mouse is over the button.
  boolean hovering(int mx, int my){
    if(mx >= xpos - Width/2 && mx <= xpos + Width/2){
      if(my >= ypos - Height/2 && my <= ypos + Height/2){
        return true;
      }
    }
    return false;
  }
  
} // Button class end.
