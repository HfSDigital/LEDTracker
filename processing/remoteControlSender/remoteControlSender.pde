import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress nodeMCU;

float px, py, x, y;
float sx = 0, sy = 0;
float speed = 0.3;

void setup()
{
  size(1080, 1920, P2D);

  frameRate(60);

  // OSC
  OscProperties op = new OscProperties();
  op.setListeningPort(9000);
  op.setDatagramSize(2048);
  op.setSRSP(OscProperties.ON);
  oscP5 = new OscP5(this, op);
  nodeMCU = new NetAddress("192.168.43.35", 8000);
}


void draw()
{
  background(111);

  if (mousePressed) {
    px = mouseX;
    py = mouseY;
  } else
  {
    px = width / 2;
    py = height / 2;
  }

  stroke(0);
  line(width/2, 0, width/2, height);
  line(0, height/2, width, height/2);
  noStroke();
  fill(255);

  x = lerp(x, px, speed);
  y = lerp(y, py, speed);
  ellipse(x, y, 100, 100);

  textSize(42);
  
  
  if ( dist(px, py, x, y) > 0.2) {
    sx = ((x/width - 1/width) - 0.5) * 2;
    sy = ((y/height - 1/height) - 0.5) * -2;
    text("sending... ", 10, 100);
    
    // send OSC Message
    sx = int(sx * 100) / 100.0;
    sy = int(sy * 100) / 100.0;
    OscMessage runSpeed = new OscMessage("/run");
    runSpeed.add(sy);                                   
    oscP5.send(runSpeed, nodeMCU );
  }
  
  
  text(/*sx + ", " + */sy, 10, 40);
}
