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
  
  Button smooth;
  
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
    DataX = dataX.clone();
    DataXS = dataX.clone();
    DataY = dataY.clone();
    DataYS = dataY.clone();
    
    
    // Scale data to fill entire length of graph box
    DataXS = arrayScale_X(DataXS, Width);
    DataYS = arrayScale_Y(DataYS, ypos, Height);
    // Adjust array by xpos & ypos
    DataXS = arrayShift(DataXS, xpos);
    DataYS = arrayShift(DataYS, ypos);
    // Set Y values to correct coordinate system.
    DataYS = arrayFlip(DataYS, ypos);
    
    smooth = new Button(xpos+Width/2, ypos+55, 125, 50, ucscBlue, greenText, lightGrey, "Example");

  }
  
  public void display(){
    
    // Write Axis & Title Text
    drawLabels(xpos, ypos, Width, Height);
    // Draw graph box:
    drawBox(xpos, ypos, Width, Height);
    drawGrid(xpos, ypos, Width, Height);
    drawTickVals(DataX, xpos, ypos, Width);
    
    // Draw graph line
    for(int i = 0; i < DataXS.length-1; i++){
      strokeWeight(2);
      stroke(lineColor);
      line(DataXS[i], DataYS[i], DataXS[i+1], DataYS[i+1]);
    }
    
    smooth.display();
  } // Display end
  
  
  // HELPER FUNCTIONS:
  
  // Check if the was clicked location has an action
  void click(int mx, int my){
    if(smooth.wasClicked(mx, my)){ // button action.
      exit();
    }
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
    fill(ucscBlue);
    textAlign(CENTER, CENTER);
    textSize(30);
    text(tText, x+(w/2), y-h-25);
    textSize(20);
    text(xText, x+(w/2), y+35);
    pushMatrix();
    translate(x,y);
    rotate(-HALF_PI);
    text(yText, h/2, -35);
    popMatrix(); 
  }
  
  // Draw tick values
  private void drawTickVals(float[] data, int x, int y, int w){
    fill(ucscBlue);
    textAlign(CENTER, CENTER);
    textSize(10);
    
    // Horizontal
    float lastVal = data[data.length-1];
    //println(data.length-1);
    //println(data[10]);
    //println(lastVal);
    for(int i=0; i < 11; i++){
      text(int((lastVal/10)*i), x+(i*(w/10)), y+10);
    }
    
    // Vertical
    
  }
  
  // Draw Grid
  private void drawGrid(int x, int y, int w, int h){
    strokeWeight(1);
    stroke(gridLine);
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
    strokeWeight(1);
    stroke(gridLine);
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



 
