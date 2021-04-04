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

typedef const int BytePins[8];

static BytePins addressPins = { 10, 9, 8, 7, 6, 5, 4, 3 };
static BytePins ioPins = { 11, 12, 13, 2, A0, A1, A2, A3 };
static const int writeEnable  = A5;
static const int outputEnable = A4;
static const byte resetHighAddr = 0xfd;
static const byte resetLowAddr = 0xfc;

enum Mode {
    ReadMode,
    WriteMode,
    WriteResetVectorMode
};

static void
digitalWriteByte (const BytePins &pins, byte value)
{
    // Serial.println(value, HEX);
    for (int i = 0; i < 8; ++i) {
        // Serial.print(" ");
        // Serial.print(i);
        // Serial.print(" ");
        // Serial.println(value & (1 << i) ? HIGH : LOW);
        digitalWrite (pins[i], value & (1 << i) ? HIGH : LOW);
    }
}

static void
digitalWriteByte (byte address, byte value)
{
    digitalWriteByte (addressPins, address);
    digitalWriteByte (ioPins, value);
    digitalWrite (writeEnable, LOW);
    delayMicroseconds (1);
    digitalWrite (writeEnable, HIGH);
    delay (10);
}

static byte
digitalReadByte (byte address)
{
    byte b = 0;
    digitalWriteByte (addressPins, address);
    for (int i = 0; i < 8; ++i) {
        b |= digitalRead (ioPins[i]) << i;
    }
    return b;
}

static void
dumpROM ()
{
    char line[81];
    byte data[16];

    digitalWrite (outputEnable, LOW);

    for (int i = 0; i < 8; ++i) {
        pinMode (ioPins[i], INPUT);
    }

    for (byte base = 0; ; base += sizeof(data)) {
        for (byte i = 0; i < sizeof(data); ++i) {
            data[i] = digitalReadByte(base + i);
        }
        snprintf(line, sizeof(line),
                 "%08hhx: "
                 "%02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx "
                 "%02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx",
                 base,
                 data[0], data[1], data[2], data[3],
                 data[4], data[5], data[6], data[7],
                 data[8], data[9], data[10], data[11],
                 data[12], data[13], data[14], data[15]);
        Serial.println(line);
        if (base == 240) {
            return;
        }
    }
}

static void
writePROM ()
{
    byte addr = 0;
    do {
        digitalWriteByte (addr, addr);
        ++addr;
    } while (addr);
}

static void
writeResetVector ()
{
    digitalWriteByte (resetLowAddr, 0x00);
    digitalWriteByte (resetHighAddr, 0x80);
}

void
setup ()
{
    Mode mode = WriteMode;

    digitalWrite (writeEnable, HIGH);
    digitalWrite (outputEnable, HIGH);
    
    pinMode (writeEnable, OUTPUT);
    pinMode (outputEnable, OUTPUT);

    for (int i = 0; i < 8; ++i) {
        pinMode (addressPins[i], OUTPUT);
        pinMode (ioPins[i], OUTPUT);
    }

    Serial.begin (57600);

    switch (mode) {
        case WriteMode:
            writePROM ();
            break;

        case WriteResetVectorMode:
            writeResetVector ();
            break;
    }

    dumpROM ();
}

void loop () { }
