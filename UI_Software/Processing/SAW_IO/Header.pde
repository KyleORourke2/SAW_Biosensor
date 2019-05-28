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
  color fillColor;
  
  Button exitButton; // Button to close program.
  
  Header(){
    exitButton = new Button(width-25, headerHeight/2, 50, headerHeight-1, exitFill, exitText, exitStroke, "X");
  }
  
  void display(){
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
  boolean wasClicked(int mx, int my){
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
