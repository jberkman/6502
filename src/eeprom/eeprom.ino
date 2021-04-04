/*

 Pin layout:

 0        WE  A5
 1        OE  A4
 2 IO3    IO7 A3
 3 A7     IO6 A2
 4 A6     IO5 A1
 5 A5     IO4 A0
 6 A4         Aref
 7 A3         Vin
 8 A2         GND
 9 A1         GND
10 A0         5V
11 IO0        3V
12 IO1        Rst
13 IO2        USB

*/

static const int addressPins[8] = { 10, 9, 8, 7, 6, 5, 4, 3 };
static const int ioPins[8] = { 11, 12, 13, 2, A0, A1, A2, A3 };
static const int writeEnable  = A5;
static const int outputEnable = A4;

static void
digitalWriteByte(const int &pins[8], int value)
{
    for (int i = 0; i < 8; ++i) {
        digitalWrite(pins[i], value & (1 << i) ? HIGH : LOW);
    }
}

void
setup ()
{
    for (int i = 0; i < 8; ++i) {
        pinMode(addressPins[i], OUTPUT);
        pinMode(ioPins[i], OUTPUT);
    }
    pinMode(writeEnable, OUTPUT);
    pinMode(outputEnable, OUTPUT);
}

void
loop ()
{
}
