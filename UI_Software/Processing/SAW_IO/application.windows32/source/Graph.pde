// Graph class:
// Takes in data object or function and plots it on
// an X Y plot.


class Graph {
  
  color lineColor;
  color graphColor;
  String xText;
  String yText;
  int xpos;
  int ypos;
  int Width;
  int Height;
  
  // Constructor
  // positions x y, dimensions w h, color body, color text
  Graph(int data[], int x, int y, int w, int h, color lineC, String xT, String yT){
    lineColor = lineC;
    xText = xT;
    yText = yT;
    xpos = x;
    ypos = y;
    Width = w;
    Height = h;
  }
  
  void display(){
    
  }
}
