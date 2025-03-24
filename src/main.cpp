#include <Arduino.h>
#include "MAX72xxMatrix.h"
#include "MemoryFree.h"

#define DEBUG

MAX72xxMatrix matrix = MAX72xxMatrix(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules
unsigned long delayms = 1000;

uint8_t **nextState;
uint8_t newRound = 0;

void setPanelLed(uint8_t panel, uint8_t x, uint8_t y, bool state);
bool getPanelLed(uint8_t panel, uint8_t x, uint8_t y);
void createRandomPanel(uint8_t panel);
uint8_t countNeighbours(uint8_t p, uint8_t x, uint8_t y);
void GoL();

void setNextledstate(uint8_t addr, uint8_t row, uint8_t col, boolean state)
{
    uint8_t tmpval = 0;

    // check boundaries
    if (addr < 0 || addr > 24)
        return;
    if (row < 0 || row > 7)
        return;
    if (col < 0 || col > 7)
        return;

    // tmpval = pow(2,col);
    tmpval = 1 << col;

    if (state == 1)
    {
      nextState[addr][row] |= tmpval;
    }
    else
    {
      nextState[addr][row] &= (~tmpval);
    }
}

boolean getNextledstate(uint8_t addr, uint8_t row, uint8_t col)
{
    uint8_t tmpval = 0;
    // check boundaries
    if (addr < 0 || addr > 24)
        return 0;
    if (row < 0 || row > 7)
        return 0;
    if (col < 0 || col > 7)
        return 0;

    tmpval = 1 << col;
    return (nextState[addr][row] & (~tmpval));
}

void setNextPanelLedState(uint8_t panel, uint8_t x, uint8_t y, bool state)
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
    mx = 7 - x;
    my = 7 - y;
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

  setNextledstate(mp, my, mx, state);
}

bool getNextPanelLedState(uint8_t panel, uint8_t x, uint8_t y)
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
    mx = 7 - x;
    my = 7 - y;
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

  return getNextledstate(mp, my, mx);
}

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("setup start");
#endif
  int devices = matrix.getNumDevices();
#ifdef DEBUG
  Serial.print("1 ");
  Serial.println(freeMemory());
#endif
  pinMode(6, INPUT);
  randomSeed(analogRead(6));
  // allocate memory for nextState for 24 panels (8rows with 8bit (uint8_t) for each device)
  // uint8_t ledstate[addr][row]
  nextState = (uint8_t **)malloc(24 * sizeof(uint8_t *));
  // if(ledstate == NULL) ... for now I will ignore this errorhandling
  for (uint8_t i = 0; i < 24; i++)
  {
    nextState[i] = (uint8_t *)malloc(8 * sizeof(uint8_t));
    // if(ledstate[i] == NULL) ... for now I will ignore this errorhandling
  }

  for (int addr = 0; addr < devices; addr++)
  {
    matrix.setShutdown(addr, false);
    matrix.setIntensity(addr, 1);
    matrix.clearDisplay(addr);
  }
  matrix.show();
  newRound = 1;

  // allocate memory for ledstate for numDevices (8rows with 8bit (uint8_t) for each device)
  // uint8_t ledstate[addr][row]
  nextState = (uint8_t **)malloc(matrix.getNumDevices() * sizeof(uint8_t *));
  // if(ledstate == NULL) ... for now I will ignore this errorhandling
  for (uint8_t i = 0; i < matrix.getNumDevices(); i++)
  {
    nextState[i] = (uint8_t *)malloc(8 * sizeof(uint8_t));
    // if(ledstate[i] == NULL) ... for now I will ignore this errorhandling
  }

#ifdef DEBUG
  Serial.print("2 ");
  Serial.println(freeMemory());
#endif
#ifdef DEBUG
  Serial.println("setup end");
#endif
}

void loop()
{
#ifdef DEBUG
  Serial.print("3 ");
  Serial.println(freeMemory());
#endif

  if (newRound == 1)
  {
    for (uint8_t p = 0; p < 6; p++)
    {
      createRandomPanel(p);
#ifdef DEBUG
      Serial.print("4 ");
      Serial.println(freeMemory());
#endif
    }
    newRound = 0;
  }
  else
  {
    GoL();
#ifdef DEBUG
    Serial.print("5 ");
    Serial.println(freeMemory());
#endif
  }
  matrix.show();
  delay(delayms);

#ifdef DEBUG
  Serial.print("6 ");
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
    mx = 7 - x;
    my = 7 - y;
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
    mx = 7 - x;
    my = 7 - y;
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

void createRandomPanel(uint8_t panel)
{
  if (panel > 6) // cube: 6 panels
    panel = 6;

  for (uint8_t x = 0; x < 16; x++)
  {
    for (uint8_t y = 0; y < 16; y++)
    {
      if (random(100) < 60)
        setPanelLed(panel, x, y, false);
      else
        setPanelLed(panel, x, y, true);
    }
  }
}

/*
The rules for Conway's Game of Life are as follows:

(a) Any live cell with fewer than two live neighbors dies, as if by underpopulation.
(b) Any live cell with two or three live neighbors lives on to the next generation.
(c) Any live cell with more than three live neighbors dies, as if by overpopulation.
(d) Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
[(e) Any dead cell with not three live neighbours stays dead]
*/
void GoL()
{
  uint8_t p = 0, x = 0, y = 0; // cell to be evaluated
  uint8_t n = 0;               // neighbours
  uint8_t c = 0;               // c=true: live cell, false: dead cell

  // count Neighbours and set cell for next generation
  for (p = 0; p < 6; p++)
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
            c = false;
          }
          // else (b)
        }
        else
        {
          if (n == 3)
          { // (d)
            c = true;
          }
          // else (e)
        }
        setNextPanelLedState(p,x,y,c);
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
        matrix.setLed(p,x,y,getNextPanelLedState(p,x,y));
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