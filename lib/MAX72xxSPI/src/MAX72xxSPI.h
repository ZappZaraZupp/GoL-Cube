/*
 *    MAX72xxSPI.h
 *    Control chained 72xx
 *    02 2025 C. Pohl
 */

#ifndef MAX72xxSPI_h
#define MAX72xxSPI_h

#include <Arduino.h>
#include <SPI.h>

// Registers from datasheet

// Register Address Map (D11 - D08)
#define NOOP 0x00

// Digits (Rows)
#define Digit0 0x01 // row0
#define Digit1 0x02 // .
#define Digit2 0x03 // .
#define Digit3 0x04 // .
#define Digit4 0x05 // .
#define Digit5 0x06 // .
#define Digit6 0x07 // .
#define Digit7 0x08 // row7

// BCD decode (example values)
// 0x00 (b00000000): no decode 
// 0x01 (b00000001): decode digit 0, no decode 7,6,5,4,3,2,1
// 0x0f (b00001111): decode digits 3,2,1,0, no decode 7,6,5,4
// 0x83 (b10000011): decode digti 7,1,0, no decode 6,5,4,3,2
// 0xff (b11111111): decode digits 0-7
#define DECODEMODE 0x09 
// Intensity: 16 values
// 0xX0: min
// ...
// 0xXF: max
#define INTENSITY 0x0a

// set how many digits are displayed
// 0xX0: digit 0
// 0xX1: digit 0 and 1
// ...
// 0xX8: digit 0 to 7
#define SCANLIMIT 0x0b

// When the MAX7219 is in shutdown mode, the scan oscillator is
// halted, all segment current sources are pulled to ground,
// and all digit drivers are pulled to V+, thereby blanking the
// display
// value 0x01 is normal operation
#define SHUTDOWN 0x0c

// test
// value 0x00 is normal operation+
#define DISPLAYTEST 0x0f

class MAX72xxSPI
{
private:
    // data array packets (2*numDevices) 
    uint8_t* spidata;

    // connection settings
    uint8_t SPI_MOSI; //MasterOutSlaveIn
    uint8_t SPI_MISO; //MasterInSlaveOut
    uint8_t SPI_CS;   //Select
    uint8_t SPI_CLK;  //Clock
    uint8_t numDevices=0;
    // led state (8*numDevices)
    uint8_t** ledstate;

    // transfer one packet to addr
    void spiSend(uint8_t addr, uint8_t opcode, uint8_t data);

public:
    // Params:
    // dataPin		SPI_MOSI
    // csPin		SPI_CS
    // clockPin		SPI_CLK
    // numDevices	chained devices
    //MAX72xxSPI(uint8_t dataPin, uint8_t csPin, uint8_t clkPin,  uint8_t numDevices = 1);
    MAX72xxSPI(uint8_t mosiPin, uint8_t misoPin, uint8_t csPin, uint8_t clkPin,  uint8_t numDevices = 1);

    // ** getNumDevices
    // Returns:
    // int The number of attached (chained) devices
    uint8_t getNumDevices();

    // ** setShutdown
    // Params:
    // addr The address of the display in chain
    // status 0: normal op; 1: shutdown
    void setShutdown(uint8_t addr, bool status);

    // setScanLimit
    // set the number of rows to be displayed
    // Params:
    // addr The address of the display in chain
    // number Number of rows to be displayed (0..7)
    void setScanLimit(uint8_t addr, uint8_t limit);

    // setIntensity
    // Set the brightness of the display.
    // Params:
    // addr The address of the display in chain
    // intensity The brightness of the display. (0..15)
    void setIntensity(uint8_t addr, uint8_t intensity);

    // setDecode
    // Set the DecodeMode of the display.
    // Params:
    // addr The address of the display in chain
    // mode decodemode
    void setDecode(uint8_t addr, uint8_t mode);

    // setTest
    // Set the TestMode of the display.
    // Params:
    // addr The address of the display in chain
    // mode 0: testmode off 1: on
    void setTest(uint8_t addr, bool mode);

    // clearDisplay
    // all LED off
    // does not send data to devices; use show() to send to devices
    // Params:
    // addr The address of the display in chain
    void clearDisplay(uint8_t addr);

    // setLed
    // switch one LED
    // does not send data to devices; use show() if all Led are set
    // Params:
    // addr The address of the display in chain
    // row	row of the Led (0..7)
    // col	column of the Led (0..7)
    // state 1: LED on; 0: LED off
    void setLed(uint8_t addr, uint8_t row, uint8_t col, boolean state);

    // setRow
    // switch all LED in one row 
    // does not send data to devices; use show() if all rows are set
    // Params:
    // addr The address of the display in chain
    // row row which is to be set (0..7)
    // value each bit set to 1 will light up
    void setRow(uint8_t addr, uint8_t row, uint8_t value);

    // getLed
    // read ledstate[][]
    // reads local array, could be different from actually displayed LED; e.g. if show() was not called after setLed() or setRow()
    // Params:
    // addr The address of the display in chain
    // row row of the Led (0..7)
    // col column of the Led (0..7)
    boolean getLed(uint8_t addr, uint8_t row, uint8_t col);

    // send display data
    // transfer all rows to all devices
    void show(void);
};

#endif // MAX72xxSPI_h.h
