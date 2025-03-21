#include <Arduino.h>
#include "MAX72xxMatrix.h"
#include "MemoryFree.h"

//#define DEBUG

MAX72xxMatrix matrix = MAX72xxMatrix(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules
unsigned long delayms = 0;

void setPanelLed(uint8_t panel, uint8_t x, uint8_t y, bool state);
bool getPanelLed(uint8_t panel, uint8_t x, uint8_t y);

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("setup start");
#endif
  int devices = matrix.getNumDevices();
#ifdef DEBUG
  Serial.println(freeMemory());
#endif

  for (int addr = 0; addr < devices; addr++)
  {
    matrix.setShutdown(addr, false);
    matrix.setIntensity(addr, 1);
    matrix.clearDisplay(addr);
  }
  matrix.show();
#ifdef DEBUG
  Serial.println(freeMemory());
#endif
#ifdef DEBUG
  Serial.println("setup end");
#endif
}

void loop()
{
#ifdef DEBUG
  Serial.println(freeMemory());
#endif
  for(uint8_t y=0; y<16; y++) {
    for(uint8_t x=0; x<16; x++) {
      setPanelLed(0,x,y,true);
      setPanelLed(1,y,x,true);
      setPanelLed(2,15-x,y,true);
      setPanelLed(3,15-x,15-y,true);
      setPanelLed(4,x,15-y,true);
      setPanelLed(5,15-y,x,true);
      matrix.show();
      delay(delayms);
      setPanelLed(0,x,y,false);
      setPanelLed(1,y,x,false);
      setPanelLed(2,15-x,y,false);
      setPanelLed(3,15-x,15-y,false);
      setPanelLed(4,x,15-y,false);
      setPanelLed(5,15-y,x,false);
      matrix.show();
    }
  }

#ifdef DEBUG
  Serial.println(freeMemory());
#endif
}

// A cube, each side (panel) has 4 matrices of 8x8 forming a 16x16 matrix
// realised with 24 8x8 matrices, MAX 7219, daisychaned
// cube panels:
//    p4
// p0 p1 p2 p3
//    p5
//
// LED coordinates:
// 6 panels with 16x16 LEDs
// origin (0/0) is lower left on each panel
// 
// each panel consits of 4 8x8 matrices daisychained:
// |-> m2 -> m3 -> data out - 0/0 each bottom left
// |-- m1 <- m0 <- data in  - 0/0 each top right
// to physical coordinates
// p0          p1          p2    .... p5
// m0 m1 m2 m3 m4 m5 m6 m7 m8 m9 .... m20 m21 m22 m23
void setPanelLed(uint8_t panel, uint8_t x, uint8_t y, bool state)
{
  uint8_t width = 16;
  uint8_t height = 16;
  
  uint8_t mx = 0; // column of phys matrix
  uint8_t my = 0; // row of phys matrix
  uint8_t mp = 0; // addr of phys matrix

  // check borders
  // all is uint8_t --> no check if below zero needed
  if (x > width)
    x = width;
  if (y > height)
    y = height;
  if (panel > 6) // cube: 6 panels
    panel = 6;

  // lower left
  if (x < 8 && y < 8)
  {
    mp = 1;
    mx = 7-x;
    my = 7-y;
  }
  // lower right
  else if (x < 16 && y < 8)
  {
    mp = 0;
    mx = 7 - (x - 8);
    my = 7 - y;
  }
  // upper left
  else if (x < 8 && y < 16)
  {
    mp = 2;
    mx = x;
    my = y - 8;
  }
  else
  // upper right
  { // (x<16 && y<16)
    mp = 3;
    mx = x - 8;
    my = y - 8;
  }
  // move all to correct addr in chain
  mp = mp + panel * 4;

  matrix.setLed(mp, my, mx, state);
}

bool getPanelLed(uint8_t panel, uint8_t x, uint8_t y)
{
  uint8_t width = 16;
  uint8_t height = 16;
  
  uint8_t mx = 0; // column of phys matrix
  uint8_t my = 0; // row of phys matrix
  uint8_t mp = 0; // addr of phys matrix

  // check borders
  // all is uint8_t --> no check if below zero needed
  if (x > width)
    x = width;
  if (y > height)
    y = height;
  if (panel > 6) // cube: 6 panels
    panel = 6;

  // lower left
  if (x < 8 && y < 8)
  {
    mp = 1;
    mx = 7-x;
    my = 7-y;
  }
  // lower right
  else if (x < 16 && y < 8)
  {
    mp = 0;
    mx = 7 - (x - 8);
    my = 7 - y;
  }
  // upper left
  else if (x < 8 && y < 16)
  {
    mp = 2;
    mx = x;
    my = y - 8;
  }
  else
  // upper right
  { // (x<16 && y<16)
    mp = 3;
    mx = x - 8;
    my = y - 8;
  }
  // move all to correct addr in chain
  mp = mp + panel * 4;

  return matrix.getLed(mp, my, mx);
}

