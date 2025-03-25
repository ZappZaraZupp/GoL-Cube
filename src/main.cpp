#include <GoLCube.h>

#define DEBUG

MAX72xxSPI matrix = MAX72xxSPI(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules
unsigned long delayms = 1000;

uint8_t **nextState;
uint8_t newRound = 0;

void setPanelLed(CubeCoordinates c, bool state);
bool getPanelLed(CubeCoordinates);

void setup()
{
  Serial.begin(9600);
  Serial.println("setup start");

  uint8_t devices = matrix.getNumDevices();

  Serial.println(freeMemory());

  pinMode(6, INPUT);
  randomSeed(analogRead(6));
  delay(100);

  for (int addr = 0; addr < devices; addr++)
  {
    matrix.setShutdown(addr, false);
    matrix.setIntensity(addr, 1);
    matrix.clearDisplay(addr);
  }
  matrix.show();

  Serial.println("setup end");
}

void loop()
{
  for(uint8_t p=0; p<6; p++) {
    for(uint8_t x=0; x<16; x++) {
      for(uint8_t y=0; y<16; y++) {
        setPanelLed(CubeCoordinates{.panel=p,.x=x,.y=y},true);
        matrix.show();
        delay(10);
        setPanelLed(CubeCoordinates{.panel=p,.x=x,.y=y},false);
        matrix.show();
      }
    }
  }
}

void setPanelLed(CubeCoordinates c, bool state)
{
  ModuleCoordinates m;
  m = Cube2Module(c);
  matrix.setLed(m.addr, m.row, m.column, state);
}

bool getPanelLed(CubeCoordinates c)
{
  ModuleCoordinates m;
  m = Cube2Module(c);
  return matrix.getLed(m.addr, m.row, m.column);
}

/*
The rules for Conway's Game of Life are as follows:

(a) Any live cell with fewer than two live neighbors dies, as if by underpopulation.
(b) Any live cell with two or three live neighbors lives on to the next generation.
(c) Any live cell with more than three live neighbors dies, as if by overpopulation.
(d) Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
[(e) Any dead cell with not three live neighbours stays dead]

void GoL()
{
  Serial.println("GoL start");
  uint8_t p = 0, x = 0, y = 0; // cell to be evaluated
  uint8_t n = 0;               // neighbours
  uint8_t c = 0, nc = 0;       // c= current, nc= nextGen; true: live cell, false: dead cell

  // count Neighbours and set cell for next generation
  for (p = 0; p < 1; p++)
  {
    for (x = 0; x < 16; x++)
    {
      for (y = 0; y < 16; y++)
      {
        n = countNeighbours(p, x, y);
        c = matrix.getLed(p, x, y);
        if (c == true)
        {
          if (n < 2 || n > 3)
          { // (a) or (c)
            nc = false;
          }
          else
          {
            nc = true;
          }
        }
        else
        {
          if (n == 3)
          { // (d)
            nc = true;
          }
          else
          {
            nc = false;
          }
        }
        setNextPanelLedState(p, x, y, nc);
        Serial.println(String(p) + "-" + String(x) + "-" + String(y) + "-" + String(c) + "-" + String(nc) + "-" + String(n));
      }
    }
  }

  // push next generation to panels
  for (p = 0; p < 1; p++)
  {
    for (x = 0; x < 16; x++)
    {
      for (y = 0; y < 16; y++)
      {
        matrix.setLed(p, x, y, getNextPanelLedState(p, x, y));
      }
    }
  }
}

uint8_t countNeighbours(uint8_t p, uint8_t x, uint8_t y)
{
  uint8_t n = 0; // Neighbours
  int8_t ix = 0, iy = 0, cx = 0, cy = 0, cp = 0;
  for (ix = -1; ix <= 1; ix++)
  {
    for (iy = -1; iy <= 1; iy++)
    {
      if (ix != 0 || iy != 0)
      { // only neighbours
        cx = x + ix;
        cy = y + iy;
        cp = p;
        if (cx >= 0 && cx <= 15 && cy >= 0 && cy <= 15)
        { // inside panel
          if (matrix.getLed(cp, cx, cy) == true)
            n += 1;
        }
        else
        { // corners -> cell to check is on another panel
          // test: corners are dead
        }
      }
    }
  }
  return n;
}
*/