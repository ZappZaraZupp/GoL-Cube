#include <Arduino.h>
#include "MAX72xxSPI.h"
#include "MemoryFree.h"

MAX72xxSPI matrix = MAX72xxSPI(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules


struct CubeCoordinates
{
  uint8_t panel;
  uint8_t x;
  uint8_t y;
};
struct ModuleCoordinates
{
  uint8_t addr;
  uint8_t row;
  uint8_t column;
};

// Convert Cube Coordinates to LED Module Coordinates
// Input Cube Coordinates
// Panel 0..5
// each Panel:
// x00/y15 .. x15/y15
// ..
// X00/y00 .. X15/y00
// Modules in each panel are chained as follows
// > m2 m3 -> output - row0/col0 m2 and m3 at top right
// ^ m1 m0 <- input  - row0/col0 m0 and m1 at bottom left
//
// to modul daisychain coordinates
// p0          p1          p2    .... p5
// m0 m1 m2 m3 m4 m5 m6 m7 m8 m9 .... m20 m21 m22 m23
ModuleCoordinates Cube2Module(CubeCoordinates c)
{
  ModuleCoordinates m;
  // check borders
  // all is uint8_t --> no need to check if below zero
  if (c.x > 15)
    c.x = 15;
  if (c.y > 15)
    c.y = 15;
  if (c.panel > 5) // cube: 6 panels
    c.panel = 5;

  // lower left
  if (c.x < 8 && c.y < 8)
  {
    m.addr = 1;
    m.column = c.x;
    m.row = c.y;
  }
  // lower right
  else if (c.x < 16 && c.y < 8)
  {
    m.addr = 0;
    m.column = (c.x - 8);
    m.row = c.y;
  }
  // upper left
  else if (c.x < 8 && c.y < 16)
  {
    m.addr = 2;
    m.column = 7 - c.x;
    m.row = 7 - (c.y - 8);
  }
  else
  // upper right
  { // (x<16 && y<16)
    m.addr = 3;
    m.column = 7 - (c.x - 8);
    m.row = 7 - (c.y - 8);
  }
  // move all to correct addr in chain
  m.addr = m.addr + c.panel * 4;

  return m;
}

// Convert Module Coordinates to Cube
// not needed for now
CubeCoordinates Module2Cube(ModuleCoordinates m)
{
  CubeCoordinates c;
  c.panel = 0;
  c.x = 0;
  c.y = 0;
  return c;
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

void createTestPattern(void)
{
  uint16_t pattern[][16] = {
      {0b1000000110000001,
       0b1100001001000010,
       0b1010001001000100,
       0b1001001001001000,
       0b1000100110010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111},
      {0b1000000110000001,
       0b1100001010000010,
       0b1010000010000100,
       0b1001000010001000,
       0b1000100010010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111},
      {0b1000000110000001,
       0b1100001001000010,
       0b1010000010000100,
       0b1001000100001000,
       0b1000101111010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111},
      {0b1000000110000001,
       0b1100001001000010,
       0b1010000010000100,
       0b1001001001001000,
       0b1000100110010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111},
      {0b1000001000000001,
       0b1100001010000010,
       0b1010001111000100,
       0b1001000010001000,
       0b1000100010010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111},
      {0b1000001111000001,
       0b1100001000000010,
       0b1010001110000100,
       0b1001000001001000,
       0b1000101110010000,
       0b1000010000100000,
       0b1000001001000000,
       0b1000000110000000,
       0b1000000110000000,
       0b1000001001000000,
       0b1000010000100000,
       0b1000100000010000,
       0b1001000000001000,
       0b1010000000000100,
       0b1100000000000010,
       0b1111111111111111}};
  bool v = 0;

  for (uint8_t p = 0; p < 6; p++)
  {
    for (uint8_t x = 0; x < 16; x++)
    {
      for (uint8_t y = 0; y < 16; y++)
      {
        v = pattern[p][15 - y] & (1 << (15 - x));
        setPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y}, v);
      }
    }
  }
}

void createPattern(uint8_t pat)
{
  uint16_t pattern[][16] = {
      {0b1111111111111111,
       0b1000000000000001,
       0b1000000000000001,
       0b1001111111111001,
       0b1001000000001001,
       0b1001000000001001,
       0b1001001111001001,
       0b1001001001001001,
       0b1001001001001001,
       0b1001001111001001,
       0b1001000000001001,
       0b1001000000001001,
       0b1001111111111001,
       0b1000000000000001,
       0b1000000000000001,
       0b1111111111111111},

      {0b0000000000000000,
       0b0111111111111110,
       0b0100000000000010,
       0b0100000000000010,
       0b0100111111110010,
       0b0100100000010010,
       0b0100100000010010,
       0b0100100110010010,
       0b0100100110010010,
       0b0100100000010010,
       0b0100100000010010,
       0b0100111111110010,
       0b0100000000000010,
       0b0100000000000010,
       0b0111111111111110,
       0b0000000000000000},

      {0b0000000000000000,
       0b0000000000000000,
       0b0011111111111100,
       0b0010000000000100,
       0b0010000000000100,
       0b0010011111100100,
       0b0010010000100100,
       0b0010010000100100,
       0b0010010000100100,
       0b0010010000100100,
       0b0010011111100100,
       0b0010000000000100,
       0b0010000000000100,
       0b0011111111111100,
       0b0000000000000000,
       0b0000000000000000},

      {0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000,
       0b0000000000000000},

      {0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111,
       0b1111111111111111}
      };

  bool v = 0;

  for (uint8_t p = 0; p < 6; p++)
  {
    for (uint8_t x = 0; x < 16; x++)
    {
      for (uint8_t y = 0; y < 16; y++)
      {
        v = pattern[pat][15 - y] & (1 << (15 - x));
        setPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y}, v);
      }
    }
  }
}

void createRandomPattern()
{

  bool v = 0;
  uint16_t limit= 10 + (int)(analogRead(A3)/1023.0*80.0); // 10..90
  //Serial.println(limit);

  for (uint8_t p = 0; p < 6; p++)
  {
    for (uint8_t x = 0; x < 16; x++)
    {
      for (uint8_t y = 0; y < 16; y++)
      {

        if (random(100) <= limit)
        {
          v = true;
        }
        else
        {
          v = false;
        }
        setPanelLed(CubeCoordinates{.panel = p, .x = x, .y = y}, v);
      }
    }
  }
}

// panels are aranged
// see PDF for details on this ;-)
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
        // check edges
        if (ix < 0 && (0 <= iy && iy <= 15))
        {
          if (c.panel == 0)
          {
            cp = 3;
            cx = 0;
            cy = 15 - iy;
          }
          if (c.panel == 1)
          {
            cp = 5;
            cx = 15;
            cy = iy;
          }
          if (c.panel == 2)
          {
            cp = 0;
            cx = iy;
            cy = 0;
          }
          if (c.panel == 3)
          {
            cp = 0;
            cx = 0;
            cy = 15 - iy;
          }
          if (c.panel == 4)
          {
            cp = 2;
            cx = 15;
            cy = iy;
          }
          if (c.panel == 5)
          {
            cp = 3;
            cx = iy;
            cy = 0;
          }
        }
        else if (ix > 15 && (0 <= iy && iy <= 15))
        {
          if (c.panel == 0)
          {
            cp = 1;
            cx = 15 - iy;
            cy = 0;
          }
          if (c.panel == 1)
          {
            cp = 2;
            cx = iy;
            cy = 15;
          }
          if (c.panel == 2)
          {
            cp = 4;
            cx = 0;
            cy = iy;
          }
          if (c.panel == 3)
          {
            cp = 4;
            cx = 15 - iy;
            cy = 0;
          }
          if (c.panel == 4)
          {
            cp = 5;
            cx = iy;
            cy = 15;
          }
          if (c.panel == 5)
          {
            cp = 1;
            cx = 0;
            cy = iy;
          }
        }
        else if (iy < 0 && (0 <= ix && ix <= 15))
        {
          if (c.panel == 0)
          {
            cp = 2;
            cx = 0;
            cy = ix;
          }
          if (c.panel == 1)
          {
            cp = 0;
            cx = 15;
            cy = 15 - ix;
          }
          if (c.panel == 2)
          {
            cp = 3;
            cx = ix;
            cy = 15;
          }
          if (c.panel == 3)
          {
            cp = 5;
            cx = 0;
            cy = ix;
          }
          if (c.panel == 4)
          {
            cp = 3;
            cx = 15;
            cy = 15 - ix;
          }
          if (c.panel == 5)
          {
            cp = 0;
            cx = ix;
            cy = 15;
          }
        }
        else if (iy > 15 && (0 <= ix && ix <= 15))
        {
          if (c.panel == 0)
          {
            cp = 5;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 1)
          {
            cp = 4;
            cx = 15 - ix;
            cy = 15;
          }
          if (c.panel == 2)
          {
            cp = 1;
            cx = 15;
            cy = ix;
          }
          if (c.panel == 3)
          {
            cp = 2;
            cx = ix;
            cy = 0;
          }
          if (c.panel == 4)
          {
            cp = 1;
            cx = 15 - ix;
            cy = 15;
          }
          if (c.panel == 5)
          {
            cp = 4;
            cx = 15;
            cy = ix;
          }
        }
        else if (ix>=0 && ix<=15 && iy>=0 && iy<=15)
        // inside panel
        {
          cp = c.panel;
          cx = ix;
          cy = iy;
        }
        else
        // the corner - the non existing cell - nothing to count
        {
          continue;
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

// The rules for Conway's Game of Life are as follows:

// (a) Any live cell with fewer than two live neighbors dies, as if by underpopulation.
// (b) Any live cell with two or three live neighbors lives on to the next generation.
// (c) Any live cell with more than three live neighbors dies, as if by overpopulation.
// (d) Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
// [(e) Any dead cell with not three live neighbours stays dead]
uint16_t GoL()
{
  // Serial.println("GoL start");
  uint8_t p = 0, x = 0, y = 0; // cell to be evaluated
  uint8_t n = 0;               // neighbours
  uint8_t c = 0, nc = 0;       // c= current, nc= nextGen; true: live cell, false: dead cell
  uint16_t nextState[6][16];   // next generation state
  uint16_t tmpval = 0;
  uint16_t changed = 0;
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
            changed += 1;
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
            changed += 1;
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
    }
  }
  return changed;
}
