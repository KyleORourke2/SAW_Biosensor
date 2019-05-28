// Graph class:
// Takes in data object or function and plots it on
// an X Y plot.


class Graph {
  
  color lineColor;
  color graphColor;
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
  
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Graph(float[] dataX, float[] dataY, int x, int y, int w, int h, color lineC, String xT, String yT, String tT){
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
      DataXS = arrayScale_X(DataX.clone(), Width);
      DataYS = arrayScale_Y(DataY.clone(), ypos, Height);
      // Adjust array by xpos & ypos
      DataXS = arrayShift(DataXS.clone(), xpos);
      DataYS = arrayShift(DataYS.clone(), ypos);
      // Set Y values to correct coordinate system.
      DataYS = arrayFlip(DataYS.clone(), ypos);
      
      // Draw graph line
      for(int i = 0; i < DataXS.length-1; i++){
        strokeWeight(1);
        stroke(lineColor);
        line(DataXS[i], DataYS[i], DataXS[i+1], DataYS[i+1]);
      }
    }
  } // Display end
  
  
  //////////////// HELPER FUNCTIONS ////////////////
  void updateDataSet(float[] dataX, float[] dataY){
    DataX = dataX.clone(); // Copies of original data to be used in helper functions.
    DataY = dataY.clone();
    DataXS = dataX.clone(); // Scaled versions of the data.
    DataYS = dataY.clone();
  }
  
  
  // Check if the was clicked location has an action
  void click(int mx, int my){

  }
  
  // Check if a click is in an action area.
  boolean wasClicked(int mx, int my){
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
        text(int(minVal+(span/10)*i), x+(i*(size/10)), y-15);
      }
      textAlign(RIGHT, CENTER);
      text("MHz", x+(10*(size/10))-2, y-15);
      textAlign(CENTER, CENTER);
    }
    else{
      textAlign(LEFT, CENTER);
      float maxVal = data[findMax(data)];
      float minVal = data[findMin(data)];
      float span = maxVal - minVal;
      text(int(maxVal)+" dB", x+5, y-Height+10);
      for(int i=1; i < 10; i++){
        text(int(minVal+(span/10)*i), x+5, y-(i*(size/10))-2);
      }
      text(int(minVal), x+5, y-20);
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
  private float [] arrayScale_X(float[] data, int w){
    float x_scaleFactor = (w/data[data.length-1]);
    data = arrayMult(data, x_scaleFactor);
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
}



 
