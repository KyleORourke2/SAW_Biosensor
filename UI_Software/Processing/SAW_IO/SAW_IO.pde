// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


Header header;

void setup() {
  fullScreen();
  background(lightGrey);
  header = new Header();
}

void draw() {
  frameRate(12);
  
  
  header.display();   // draw exit button
}


void mousePressed() {
  header.click(mouseX, mouseY);

}
