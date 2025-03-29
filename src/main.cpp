#include <GoLCube.h>

#define DEBUG

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

  createPattern(0);
  matrix.show();
  delay(1000);

  Serial.println("setup end");
}

void loop()
{
  GoL();
  matrix.show();
  delay(delayms);
}
