// Header class:
// Provides a way to populate the top header bar easily

// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


class Header {
  int Height; // Thickness of the header.
  int Width = width; // should span entire width.
  color fillColor;
  
  Button exitButton; // Button to close program.
  
  Header(){
    exitButton = new Button(width-25, headerHeight/2, 50, headerHeight, exitFill, exitText, exitStroke, "X");
  }
  
  void display(){
    // Header bar:
    noStroke();
    fill(headerFill);
    rect(0, 0, width, headerHeight);
    
    // Objects in header:
    exitButton.display();
  }
  
} // Header class end.
