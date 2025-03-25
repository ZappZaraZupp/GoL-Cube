/*
    MAX72xxSPI
    Control chained 72xx
    02 2025 C. Pohl
 */

#include "MAX72xxSPI.h"

MAX72xxSPI ::MAX72xxSPI(uint8_t mosiPin, uint8_t misoPin, uint8_t csPin, uint8_t clkPin, uint8_t num)
{
    SPI_MOSI = mosiPin;
    SPI_MISO = misoPin;
    SPI_CS = csPin;
    SPI_CLK = clkPin;

    if (num <= 0)
    {
        numDevices = 1;
    }
    else
    {
        numDevices = num;
    }

    // allocate memory for spidata for numdevices (16bit for each device)
    // uint8_t spidata[numDevices*2]
    spidata = (uint8_t *)malloc(numDevices * 2 * sizeof(uint8_t));
    // if(spidata == NULL) ... for now I will ignore this errorhandling

    // allocate memory for ledstate for numDevices (8rows with 8bit (uint8_t) for each device)
    // uint8_t ledstate[addr][row]
    ledstate = (uint8_t **)malloc(numDevices * sizeof(uint8_t *));
    // if(ledstate == NULL) ... for now I will ignore this errorhandling
    for (uint8_t i = 0; i < numDevices; i++)
    {
        ledstate[i] = (uint8_t *)malloc(8 * sizeof(uint8_t));
        // if(ledstate[i] == NULL) ... for now I will ignore this errorhandling
    }

    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_MISO, INPUT);
    pinMode(SPI_CS, OUTPUT);
    pinMode(SPI_CLK, OUTPUT);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);

    for (uint8_t i = 0; i < numDevices; i++)
    {
        // no test mode
        setTest(i, 0);
        // display all rows
        setScanLimit(i, 7);
        // no on chip decode
        setDecode(i, 0);
        // clear all displays
        clearDisplay(i);
        // all on power-saving mode
        setShutdown(i, true);
    }
}

uint8_t MAX72xxSPI ::getNumDevices()
{
    return numDevices;
}

void MAX72xxSPI ::setShutdown(uint8_t addr, bool b)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;

    if (b)
        spiSend(addr, SHUTDOWN, 0x00);
    else
        spiSend(addr, SHUTDOWN, 0x01);
}

void MAX72xxSPI ::setDecode(uint8_t addr, uint8_t val)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;

    if (val >= 0 && val <= 0xff)
        spiSend(addr, DECODEMODE, val);
}

void MAX72xxSPI ::setTest(uint8_t addr, bool b)
{
    // check boundaries
    if (addr < 0 || addr >= numDevices)
        return;

    if (b)
        spiSend(addr, DISPLAYTEST, 0x01);
    else
        spiSend(addr, DISPLAYTEST, 0x00);
}

void MAX72xxSPI ::setScanLimit(uint8_t addr, uint8_t limit)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;

    if (limit >= 0 && limit < 8)
        spiSend(addr, SCANLIMIT, limit);
}

void MAX72xxSPI ::setIntensity(uint8_t addr, uint8_t intensity)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;

    if (intensity >= 0 && intensity < 16)
        spiSend(addr, INTENSITY, intensity);
}

void MAX72xxSPI ::clearDisplay(uint8_t addr)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;

    for (int i = 0; i < 8; i++)
    {
        setRow(addr, i, 0x00);
    }
}

void MAX72xxSPI ::setRow(uint8_t addr, uint8_t row, uint8_t value)
{
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;
    if (row < 0 || row > 7)
        return;

    ledstate[addr][row] = value;
    // row0 = RegisterAddress 0x01
    // row1 = RegisterAddress 0x02
    // --> thats why row + 1 ;-)
    // spiSend(addr, row + 1, ledstate[addr][row]);
}

void MAX72xxSPI ::setLed(uint8_t addr, uint8_t row, uint8_t col, boolean state)
{
    uint8_t tmpval = 0;

    // check boundaries
    if (addr < 0 || addr > numDevices)
        return;
    if (row < 0 || row > 7)
        return;
    if (col < 0 || col > 7)
        return;

    // tmpval = pow(2,col);
    tmpval = 1 << col;

    if (state == 1)
    {
        ledstate[addr][row] |= tmpval;
    }
    else
    {
        ledstate[addr][row] &= (~tmpval);
    }
}

boolean MAX72xxSPI ::getLed(uint8_t addr, uint8_t row, uint8_t col)
{
    uint8_t tmpval = 0;
    // check boundaries
    if (addr < 0 || addr > numDevices)
        return 0;
    if (row < 0 || row > 7)
        return 0;
    if (col < 0 || col > 7)
        return 0;

    tmpval = 1 << col;
    return (ledstate[addr][row] & (~tmpval));
}

// send *one* data packet (opcode and value) to addr
void MAX72xxSPI ::spiSend(uint8_t addr, volatile uint8_t opcode, volatile uint8_t data)
{
    // Create an array with the data to shift out
    // each datapacket is 16 bit (8bit opcode 8bit data)
    uint8_t datalen = numDevices * 2;
    uint8_t dataoff = addr * 2;
    for (uint8_t i = 0; i < datalen; i++)
        spidata[i] = (uint8_t)0;
    // create datapacket
    spidata[dataoff] = data;
    spidata[dataoff + 1] = opcode;
    // enable chipselect
    digitalWrite(SPI_CS, LOW);
    // send data
    // D15 is first received by 72xx (1st sent)
    // D15-D12 are ignored bits
    // D11-D8 contain the register address (opcode) --> see header file
    // D7-D0 contain the data values
    for (uint8_t i = datalen; i > 0; i--)
        //shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);
        SPI.transfer(spidata[i-1]);
    // The data is then latched into either the digit or
    // control registers on the rising edge of CS
    digitalWrite(SPI_CS, HIGH);
}

// send and show stored ledstate[][]
void MAX72xxSPI ::show()
{
    // go through ledstate array
    // build spidata array
    // send for each row

    uint8_t datalen = numDevices * 2;
    uint8_t dataoff;

    for (uint8_t row = 0; row < 8; row++)
    {
        for (uint8_t addr = 0; addr < numDevices; addr++)
        {
            dataoff = addr * 2;
            spidata[dataoff] = ledstate[addr][row]; // data
            spidata[dataoff + 1] = row + 1;         // opcode row0=0x01 row1=0x02...
        }
        // send data
        digitalWrite(SPI_CS, LOW);
        for (uint8_t i = datalen; i > 0; i--)
        {
            SPI.transfer(spidata[i-1]);
            //shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);
        }
        digitalWrite(SPI_CS, HIGH);
    }
}
