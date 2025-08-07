#include <GoLCube.h>

#define DEBUG

uint16_t oldchanged = 0;   // changed cells last round
uint8_t cnodiffrounds = 0; // number of rounds without changed cells difference
unsigned long delayms = 0;
uint8_t intensity = 6;
uint8_t ointensity = intensity;

void newCube()
{
  for (uint8_t i = 0; i < 3; i++)
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

  createRandomPattern(); // random patern
  matrix.show();
  delay(1000);
}

uint8_t getBrightness()
{
  return (uint8_t)(analogRead(A2) / 1023.0 * 15.0);
}

void setup()
{
  //Serial.begin(9600);
  //Serial.println("setup start");

  uint8_t devices = matrix.getNumDevices();

  pinMode(A0, INPUT); // random
  pinMode(A1, INPUT); // button
  pinMode(A2, INPUT); // brightness
  pinMode(A3, INPUT); // start density
  randomSeed(analogRead(A0));

  delay(100);

  intensity = getBrightness();
  ointensity = intensity;

  for (int addr = 0; addr < devices; addr++)
  {
    matrix.setShutdown(addr, false);
    matrix.setIntensity(addr, intensity);
    matrix.clearDisplay(addr);
  }
  matrix.show();
  delay(100);

  if (digitalRead(A1) == 1)
  {
    // Button pressed on startup
    Serial.println("Test");
    createTestPattern(); // test pattern for 60 seconds
    matrix.show();
    delay(60000);
    // createPattern(4); // all LED on
    // matrix.show();
    // delay(1000);
  }

  newCube();

  //Serial.println("setup end");
}

void loop()
{
  uint16_t changed = 0;
  intensity = getBrightness();
  if (intensity != ointensity) // only if changed
  {
    ointensity = intensity;
    for (int addr = 0; addr < matrix.getNumDevices(); addr++)
    {
      matrix.setIntensity(addr, intensity);
    }
  }

  /*
  Serial.print(analogRead(A0));
  Serial.print(" ");
  Serial.print(digitalRead(A1));
  Serial.print(" ");
  Serial.print(analogRead(A2));
  Serial.print(" ");
  Serial.println(analogRead(A3));
  */

  // Brightness

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
  if (digitalRead(A1) || cnodiffrounds > 10)
    newCube();

  matrix.show();
  delay(delayms);
}
