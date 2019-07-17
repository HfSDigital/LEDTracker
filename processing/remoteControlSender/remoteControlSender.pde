import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress nodeMCU;

ArrayList<ControlUnit> controlUnits = new ArrayList<ControlUnit>();

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
  nodeMCU = new NetAddress("192.168.43.255", 8000);

  controlUnits.add(new ControlUnit());
  controlUnits.add(new ControlUnit());
  controlUnits.get(0).c = color(255, 0, 0);
  controlUnits.get(1).c = color(0, 255, 0);
}


void draw()
{
  background(111);

  if (touches.length == 1) {
    for (ControlUnit cu : controlUnits) {
      cu.px = width / 2;
      cu.py = touches[0].y;
    }
  } else if (touches.length == 2) {
    if ((touches[0].x < width / 2) && (touches[1].x > width / 2))
    {
      controlUnits.get(0).px = width / 4;
      controlUnits.get(0).py = touches[0].y;
      controlUnits.get(1).px = width / 4 * 3;
      controlUnits.get(1).py = touches[1].y;
    } else if ((touches[0].x > width / 2) && (touches[1].x < width / 2))
    {
      controlUnits.get(0).px = width / 4;
      controlUnits.get(0).py = touches[1].y;
      controlUnits.get(1).px = width / 4 * 3;
      controlUnits.get(1).py = touches[0].y;
    } else {
      for (ControlUnit cu : controlUnits) {
        cu.px = width / 2;
        cu.py = height / 2;
      }
    }
  } else
  {
    for (ControlUnit cu : controlUnits) {
      cu.px = width / 2;
      cu.py = height / 2;
    }
  }


  // draw crosshair
  stroke(0);
  if (touches.length == 2) {
    line(width / 4, 0, width / 4, height);
    line(width / 4 * 3, 0, width / 4 * 3, height);
  } else {
    line(width / 2, 0, width / 2, height);
  }
  line(0, height / 2, width, height / 2);
  noStroke();
  fill(255);
  textSize(42);


  // update controlUnits
  if (controlUnits.get(0).update() && controlUnits.get(1).update())
  {
    text("sending... ", 10, 120);

    // send OSC Message
    OscMessage runSpeed = new OscMessage("/run");
    runSpeed.add(controlUnits.get(0).sy);                                   
    runSpeed.add(controlUnits.get(1).sy);                                   
    oscP5.send(runSpeed, nodeMCU );
  }

  // draw controlUnits
  controlUnits.get(0).draw(10, 40);
  controlUnits.get(1).draw(10, 80);
}
