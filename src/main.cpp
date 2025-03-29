#include <GoLCube.h>

#define DEBUG

MAX72xxSPI matrix = MAX72xxSPI(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules
unsigned long delayms = 10;
uint8_t intensity = 1; // 1..15

void createTestPattern(void);
void setPanelLed(CubeCoordinates c, bool state);
bool getPanelLed(CubeCoordinates c);
void GoL(void);
uint8_t countNeighbours(CubeCoordinates c);

void setup()
{
  Serial.begin(9600);
  Serial.println("setup start");

  uint8_t devices = matrix.getNumDevices();
  Serial.println(freeMemory());

  delay(100);

  for (int addr = 0; addr < devices; addr++)
  {
    matrix.setShutdown(addr, false);
    matrix.setIntensity(addr, intensity);
    matrix.clearDisplay(addr);
  }
  matrix.show();
  delay(100);

  createTestPattern();
  matrix.show();
  delay(1000);

  Serial.println("setup end");
}

void loop()
{
  delay(delayms);
  GoL();
  matrix.show();
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

void createTestPattern()
{
  uint16_t pattern[7][16] = {{0b0000000000000000,
                              0b0000000000011100,
                              0b1010000000100010,
                              0b0100000000100010,
                              0b0100000000100010,
                              0b0000000000011100,
                              0b0000000000000000,
                              0b1000000000000000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000000000011000,
                              0b1010000000101000,
                              0b0100000000001000,
                              0b0100000000001000,
                              0b0000000000001000,
                              0b0000000000000000,
                              0b1000000000000000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000000000000000,
                              0b1010000001110000,
                              0b0100000010001000,
                              0b0100000000010000,
                              0b0000000000100000,
                              0b0000000001000000,
                              0b1000000011111000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000000000000000,
                              0b1010000011110000,
                              0b0100000000010000,
                              0b0100000000110000,
                              0b0000000000010000,
                              0b0000000011110000,
                              0b1000000000000000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000000000000000,
                              0b1010000001000000,
                              0b0100000001010000,
                              0b0100000001111000,
                              0b0000000000010000,
                              0b0000000000010000,
                              0b1000000000000000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000000000000000,
                              0b1010000011111000,
                              0b0100000010000000,
                              0b0100000011100000,
                              0b0000000000010000,
                              0b0000000011110000,
                              0b1000000000000000,
                              0b1100000000000000,
                              0b1010000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000000000000,
                              0b1000000100010100,
                              0b1000000010001000,
                              0b1111111111010100},
                             {0b0000000000000000,
                              0b0000011000000000,
                              0b0000100100000000,
                              0b0000101000000000,
                              0b0000010000000000,
                              0b0000000000000000,
                              0b0000000000000000,
                              0b0000000000000000,
                              0b0000011000000000,
                              0b0000010100000000,
                              0b0000010000000000,
                              0b0000000000000000,
                              0b0000000000000000,
                              0b0000000000000000,
                              0b0000000000000000,
                              0b0000000000000000}};
  bool v = 0;
  for (uint8_t p = 0; p < 6; p++)
  {
    // setPanelLed(CubeCoordinates{.panel = p, .x = 0, .y = 0}, true);
    // setPanelLed(CubeCoordinates{.panel = p, .x = 0, .y = p+2}, true);

    for (uint8_t x = 0; x < 16; x++)
    {
      for (uint8_t y = 0; y < 16; y++)
      {
        v = pattern[6][15 - y] & (1 << (15 - x));
        // Serial.print(v);
        setPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y}, v);
      }
      // Serial.println("-");
    }
  }
}

// The rules for Conway's Game of Life are as follows:

// (a) Any live cell with fewer than two live neighbors dies, as if by underpopulation.
// (b) Any live cell with two or three live neighbors lives on to the next generation.
// (c) Any live cell with more than three live neighbors dies, as if by overpopulation.
// (d) Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
// [(e) Any dead cell with not three live neighbours stays dead]

void GoL()
{
  // Serial.println("GoL start");
  uint8_t p = 0, x = 0, y = 0; // cell to be evaluated
  uint8_t n = 0;               // neighbours
  uint8_t c = 0, nc = 0;       // c= current, nc= nextGen; true: live cell, false: dead cell
  uint16_t nextState[6][16];   // next generation state
  uint16_t tmpval = 0;
  bool v = 0;

  // count Neighbours and set cell for next generation
  for (p = 0; p < 6; p++)
  {
    for (x = 0; x < 16; x++)
    {
      for (y = 0; y < 16; y++)
      {
        n = countNeighbours(CubeCoordinates{.panel = p, .x = x, .y = y});
        c = getPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y});
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

        tmpval = 1 << x;

        if (nc == true)
        {
          nextState[p][y] |= tmpval;
        }
        else
        {
          nextState[p][y] &= (~tmpval);
        }
        // Serial.println(String(p) + "-" + String(x) + "-" + String(y) + "-" + String(c) + "-" + String(nc) + "-" + String(n));
      }
    }
  }

  // push next generation to panels

  for (p = 0; p < 6; p++)
  {
    for (x = 0; x < 16; x++)
    {
      for (y = 0; y < 16; y++)
      {
        tmpval = 1 << x;
        v = nextState[p][y] & (tmpval);
        setPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y}, v);
      }
      // Serial.println("-");
    }
  }
}

// panels are aranged
// P5 P4
//    P3
//    P2
//    P1 P0
// each panel has 0/0 in the lower left
uint8_t countNeighbours(CubeCoordinates c)
{
  uint8_t n = 0;                  // Number of live neighbours
  int8_t ix = 0, iy = 0;          // -1,0,1 -> go around cell to check
  uint8_t cx = 0, cy = 0, cp = 0; // is cell this live or dead

  for (ix = c.x - 1; ix <= c.x + 1; ix++)
  {
    for (iy = c.y - 1; iy <= c.y + 1; iy++)
    {
      if (ix != c.x || iy != c.y) // only neighbours, not current cell
      {

        if (ix < 0 && (0 <= iy && iy <= 15))
        {
          if (c.panel == 0)
          {
            cp = 1;
            cx = 15;
            cy = iy;
          }
          if (c.panel == 1)
          {
            cp = 5;
            cx = iy;
            cy = 15;
          }
          if (c.panel == 2)
          {
            cp = 5;
            cx = 0;
            cy = 15 - iy;
          }
          if (c.panel == 3)
          {
            cp = 5;
            cx = iy;
            cy = 0;
          }
          if (c.panel == 4)
          {
            cp = 5;
            cx = 15;
            cy = iy;
          }
          if (c.panel == 5)
          {
            cp = 2;
            cx = 0;
            cy = 15 - iy;
          }
        }
        else if (ix > 15 && (0 <= iy && iy <= 15))
        {
          if (c.panel == 0)
          {
            cp = 3;
            cx = 15;
            cy = 15 - iy;
          }
          if (c.panel == 1)
          {
            cp = 0;
            cx = 0;
            cy = iy;
          }
          if (c.panel == 2)
          {
            cp = 0;
            cx = iy;
            cy = 15;
          }
          if (c.panel == 3)
          {
            cp = 0;
            cx = 15;
            cy = 15 - iy;
          }
          if (c.panel == 4)
          {
            cp = 0;
            cx = 15 - iy;
            cy = 0;
          }
          if (c.panel == 5)
          {
            cp = 4;
            cx = 0;
            cy = iy;
          }
        }
        else if (iy < 0 && (0 <= ix && ix <= 15))
        {
          if (c.panel == 0)
          {
            cp = 4;
            cx = 15;
            cy = 15 - ix;
          }
          if (c.panel == 1)
          {
            cp = 4;
            cx = ix;
            cy = 15;
          }
          if (c.panel == 2)
          {
            cp = 1;
            cx = ix;
            cy = 15;
          }
          if (c.panel == 3)
          {
            cp = 2;
            cx = ix;
            cy = 15;
          }
          if (c.panel == 4)
          {
            cp = 3;
            cx = ix;
            cy = 15;
          }
          if (c.panel == 5)
          {
            cp = 3;
            cx = 0;
            cy = ix;
          }
        }
        else if (iy > 15 && (0 <= ix && ix <= 15))
        {
          if (c.panel == 0)
          {
            cp = 2;
            cx = 15;
            cy = ix;
          }
          if (c.panel == 1)
          {
            cp = 2;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 2)
          {
            cp = 3;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 3)
          {
            cp = 4;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 4)
          {
            cp = 1;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 5)
          {
            cp = 1;
            cx = 0;
            cy = ix;
          }
        }
        else
        {
          cp = c.panel;
          cx = ix;
          cy = iy;
        }

        if (getPanelLed(CubeCoordinates{.panel = cp, .x = cx, .y = cy}) == true)
        {
          n += 1;
        }
      }
    }
  }
  return n;
}