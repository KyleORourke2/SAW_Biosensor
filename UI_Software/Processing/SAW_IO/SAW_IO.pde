// SAW Biosensor User I/O
// Written by Kyle O'Rourke
// ktorourk@ucsc.edu
// Latest change: 2/7/2019


Header header;
Graph graph1, graph2, graph3;

PImage img;
float[] dataY1 = {-10,10,24,31,10,50,4.6,70,100,45,0}; // Example data
float[] dataY2 = {100,10,204,301,100,50,46,70,140,45,200};
float[] dataY3 = {600,500,300,100,10,50,10,70,100,45,0};
float[] dataX = {0,10,20,30,40,50,60,70,80,90,100};

void setup() {
  fullScreen();
  background(lightGrey);
  header = new Header();
  //img = loadImage("data.png");
  graph1 = new Graph (dataX, dataY1, 300, 800, 600, 400, ch1_color, "Frequency", "Y-Value", "Example Graph 1");
  graph2 = new Graph (dataX, dataY2, 1000, 800, 600, 400, ch2_color, "Frequency", "Y-Value", "Example Graph 2");
  graph3 = new Graph (dataX, dataY3, 1700, 800, 600, 400, ch3_color, "Frequency", "Y-Value", "Example Graph 2");

}

void draw() {
  frameRate(12);
  header.display();   // draw header bar
  //image(img, 350, 200, 1800, 1100);
  graph1.display();
  graph2.display();
  graph3.display();
  
}


void mousePressed() {
  header.click(mouseX, mouseY);
  graph1.click(mouseX, mouseY);
  graph2.click(mouseX, mouseY);
  graph3.click(mouseX, mouseY);
}


// Imports data from textfile into an array.
/*

//Ex:
//ball:  147.789,  134.801,  26.9737 
//ball:  112.6074,  83.0108, 11.2267 
//ball: 1175.982,  161.524,  27.2046 
//ball: 1163.498,  325.609,  19.7859 
//ball: 1115.718,  310.419,   4.00579 
//ball: 1163.047,  338.129,   1.39272 


ArrayList balls = new ArrayList();
void importData(){
  String lines[] = loadStrings("data.txt");
  for(int i = 0; i < lines.length; i++){
    if (lines[i].startsWith("ball:")){
      float values[] = float(lines[i].split(":")[1].split( ",")); //very short and ugly !
      balls.add( new Ball(values[0], values[1], values[2]) );
    }
  }
}
class Ball{
  Ball(float x, float y, float z){
    println( " generated new Ball: "+ x +", "+y+", "+z);
  }
}
*/
