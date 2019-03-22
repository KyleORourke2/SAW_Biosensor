// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


Header header;
PImage img;


void setup() {
  fullScreen();
  background(lightGrey);
  header = new Header();
  img = loadImage("data.png");
}

void draw() {
  frameRate(35);
  header.display();   // draw header bar
  image(img, 350, 200, 1800, 1100);
}


void mousePressed() {
  header.click(mouseX, mouseY);
  
}
