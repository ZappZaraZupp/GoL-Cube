#include <Arduino.h>
#include "MAX72xxSPI.h"
#include "MemoryFree.h"

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
// m2 m3 - row0/col0 each bottom left
// m1 m0 - row0/col0 each top right
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
    m.column = 7 - c.x;
    m.row = 7 - c.y;
  }
  // lower right
  else if (c.x < 16 && c.y < 8)
  {
    m.addr = 0;
    m.column = 7 - (c.x - 8);
    m.row = 7 - c.y;
  }
  // upper left
  else if (c.x < 8 && c.y < 16)
  {
    m.addr = 2;
    m.column = c.x;
    m.row = c.y - 8;
  }
  else
  // upper right
  { // (x<16 && y<16)
    m.addr = 3;
    m.column = c.x - 8;
    m.row = c.y - 8;
  }
  // move all to correct addr in chain
  m.addr = m.addr + c.panel * 4;

  return m;
}

CubeCoordinates Module2Cube(ModuleCoordinates m)
{
  CubeCoordinates c;
  c.panel = 0;
  c.x = 0;
  c.y = 0;
  return c;
}