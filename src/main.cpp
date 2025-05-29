#include <GoLCube.h>

#define DEBUG

uint16_t oldchanged = 0;   // changed cells last round
uint8_t cnodiffrounds = 0; // number of rounds without changed cells difference
unsigned long delayms = 0;

void newCube()
{
  for (uint8_t i = 0; i < 10; i++)
  {
    createPattern(1);
    matrix.show();
    delay(80);
    createPattern(0);
    matrix.show();
    delay(80);
    createPattern(2);
    matrix.show();
    delay(80);
  }
  for (uint8_t i = 0; i < 10; i++)
  {
    createPattern(3);
    matrix.show();
    delay(80);
    createPattern(4);
    matrix.show();
    delay(80);
  }

  createRandomPattern(30); // random patern
  matrix.show();
  delay(1000);
}

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
    matrix.setIntensity(addr, intensity);
    matrix.clearDisplay(addr);
  }
  matrix.show();
  delay(100);

  createTestPattern(); // test pattern 
  matrix.show();
  delay(5000);
  // createPattern(1);
  // matrix.show();
  // delay(1000);
  // createPattern(0);
  // matrix.show();
  // delay(1000);
  newCube();

  Serial.println("setup end");
}

void loop()
{
  uint16_t changed = 0;
  changed = GoL();
  if (changed - oldchanged == 0)
  {
    cnodiffrounds += 1;
  }
  else
  {
    cnodiffrounds = 0;
  }
  oldchanged = changed;
  if (cnodiffrounds > 10)
    newCube();
  matrix.show();
  delay(delayms);
}
