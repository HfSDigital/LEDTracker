class ControlUnit 
{
  float px, py, x, y;
  float sx = 0, sy = 0;
  float speed = 0.3;
  color c = color(255, 0, 0);

  ControlUnit() {}


  boolean update() {
    x = lerp(x, px, speed);
    y = lerp(y, py, speed);

    if ( dist(px, py, x, y) > 0.2) {
      sx = ((x/width - 1/width) - 0.5) * 2;
      sy = ((y/height - 1/height) - 0.5) * -2;

      sx = int(sx * 100) / 100.0;
      sy = int(sy * 100) / 100.0;
      return true;
    }
    return false;
  }


  void draw(int tx, int ty) {
    fill(c);
    ellipse(x, y, 100, 100);
    text(sx + ", " + sy, tx, ty);
  }
}
