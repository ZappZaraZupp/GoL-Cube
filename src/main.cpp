#include <GoLCube.h>

#define DEBUG

MAX72xxSPI matrix = MAX72xxSPI(11, 12, 10, 13, 24); // OUT/MOSI, IN/MISO, CS, SCK, Number of modules
unsigned long delayms = 1000;

uint16_t nextState[6][16]; //next generation state

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

  delay(100);
  createTestPattern();
  matrix.show();

  Serial.println("setup end");
}

void loop()
{
  delay(100);
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

void createTestPattern() {
  uint16_t pattern1[16]= {
    0b0000000000000000,
    0b0011000000110000,
    0b0011000000101000,
    0b0000000000110000,
    0b0000000000101000,
    0b0000000000000000,
    0b0000111000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000100000000000,
    0b0000100100000000,
    0b0000100100000000,
    0b0000000100000000,
    0b0100000000000000,
    0b0101010000000000,
    0b0101110000000000
  };
  bool v=0;
  for(uint8_t p=0; p<6; p++ ) {
    for(uint8_t x=0; x<15; x++) {
      for(uint8_t y=0; y<15; y++) {
        v=pattern1[15-y]&(1<<(15-x));
        //Serial.print(v);
        setPanelLed(CubeCoordinates{.panel=p,.x=x,.y=y},v);
      }
      //Serial.println("-");
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
  Serial.println("GoL start");
  uint8_t p = 0, x = 0, y = 0; // cell to be evaluated
  uint8_t n = 0;               // neighbours
  uint8_t c = 0, nc = 0;       // c= current, nc= nextGen; true: live cell, false: dead cell
  uint16_t tmpval = 0;
  bool v=0;

  // count Neighbours and set cell for next generation
  for (p = 0; p < 6; p++)
  {
    for (x = 0; x < 16; x++)
    {
      for (y = 0; y < 16; y++)
      {
        n = countNeighbours(CubeCoordinates{.panel=p,.x=x,.y=y});
        c = getPanelLed(CubeCoordinates{.panel=p,.x=x,.y=y});
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
        //Serial.println(String(p) + "-" + String(x) + "-" + String(y) + "-" + String(c) + "-" + String(nc) + "-" + String(n));
      }
    }
  }

  // push next generation to panels
  
  for(uint8_t p=0; p<6; p++ ) {
    for(uint8_t x=0; x<15; x++) {
      for(uint8_t y=0; y<15; y++) {
        tmpval = 1 << x;
        v = nextState[p][y] & (tmpval);
        setPanelLed(CubeCoordinates{.panel=p,.x=x,.y=y},v);
      }
      //Serial.println("-");
    }
  }
}

uint8_t countNeighbours(CubeCoordinates c)
{
  uint8_t n = 0; // Number of live neighbours
  int8_t ix = 0, iy = 0;
  uint8_t cx = 0, cy = 0, cp = 0;
  for (ix = -1; ix <= 1; ix++)
  {
    for (iy = -1; iy <= 1; iy++)
    {
      if (ix != 0 || iy != 0) // only neighbours, not current cell
      { 
        cx = c.x + ix;
        cy = c.y + iy;
        cp = c.panel;
        if (cx >= 0 && cx <= 15 && cy >= 0 && cy <= 15) // inside this panel
        { 
          if (getPanelLed(CubeCoordinates{.panel=cp,.x=cx,.y=cy}) == true) {
            n += 1;
          }
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