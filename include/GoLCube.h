#include <Arduino.h>
#include "MAX72xxSPI.h"
#include "MemoryFree.h"

void setPanelLed(uint8_t panel, uint8_t x, uint8_t y, bool state);
bool getPanelLed(uint8_t panel, uint8_t x, uint8_t y);

