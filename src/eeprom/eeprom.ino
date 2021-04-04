/*

This is similar to Ben's EEPROM programmer, but doesn't use shift
registers. Instead, this can only program a page at a time due to the
limited number of pins on the Arduino Uno.
 
Pin layout for ReadMode, WriteMode, and WriteResetVectorMode:

            0        A5    WEB 
            1        A4    OEB 
       IO3  2        A3    IO7
        A7  3        A2    IO6 
        A6  4        A1    IO5 
        A5  5        A0    IO4 
        A4  6        Aref
        A3  7        Vin
        A2  8        GND
        A1  9        GND   GND
        A0 10        5V    VCC
       IO0 11        3V
       IO1 12        Rst
       IO2 13        USB

Pin layout for ResetDataProtectionMode and EnableDataProtectionMode:

            0        A5    WEB 
            1        A4    OEB 
       IO3  2        A3    IO7
            3        A2    IO6 
       A14  4        A1    IO5 
       A13  5        A0    IO4 
       A12  6        Aref
 A3 A7 A11  7        Vin
 A2 A6 A10  8        GND
 A1 A5  A9  9        GND   GND
 A0 A4  A8 10        5V    VCC
       IO0 11        3V
       IO1 12        Rst
       IO2 13        USB

*/

typedef const int BytePins[8];

static BytePins addressPins = { 10, 9, 8, 7, 6, 5, 4, 3 };
static BytePins ioPins = { 11, 12, 13, 2, A0, A1, A2, A3 };
static const int writeEnable  = A5;
static const int outputEnable = A4;
static const byte resetHighAddr = 0xfd;
static const byte resetLowAddr = 0xfc;

enum Mode {
    // No I/O is performed; used to power board.
    PowerMode,

    // Contents of ROM are printed to serial console.
    ReadMode,

    // Write data to EEPROM.
    WriteMode,

    // Write the address for the 6502 to start execution to addresses
    // 0xfc and 0xfd.
    WriteResetVectorMode,

    // Disable software data protection mode on the EEPROM.
    ResetDataProtectionMode,

    // Enable software data protection mode on the EEPROM.
    EnableDataProtectionMode
};

static const Mode mode = PowerMode;

static void
digitalWriteByte (const BytePins &pins, byte value)
{
    for (byte i = 0; i < 8; ++i) {
        digitalWrite (pins[i], (value >> i) & 1);
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
    for (byte i = 0; i < 8; ++i) {
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

    for (byte i = 0; i < 8; ++i) {
        pinMode (ioPins[i], INPUT);
    }

    byte base = 0;
    do {
        for (byte i = 0; i < sizeof(data); ++i) {
            data[i] = digitalReadByte(base + i);
        }
        snprintf(line, sizeof(line),
                 "%08hhx: "
                 "%02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx  "
                 "%02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx",
                 base,
                 data[0], data[1], data[2], data[3],
                 data[4], data[5], data[6], data[7],
                 data[8], data[9], data[10], data[11],
                 data[12], data[13], data[14], data[15]);
        Serial.println(line);
        base += sizeof(data);
    } while (base);
}

static void
writePROM ()
{
    byte addr = 0;
    do {
        digitalWriteByte (addr, addr);
    } while (++addr);
}

static void
writeResetVector ()
{
    digitalWriteByte (resetLowAddr, 0x00);
    digitalWriteByte (resetHighAddr, 0x80);
}

static void
resetSDP ()
{
    digitalWriteByte (0x55, 0xaa);
    digitalWriteByte (0x2a, 0x55);
    digitalWriteByte (0x55, 0x80);
    digitalWriteByte (0x55, 0xaa);
    digitalWriteByte (0x2a, 0x55);
    digitalWriteByte (0x55, 0x20);
}

static void
enableSDP ()
{
    digitalWriteByte (0x55, 0xaa);
    digitalWriteByte (0x2a, 0x55);
    digitalWriteByte (0x55, 0xa0);
}

void
setup ()
{
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
        case PowerMode:
            return;

        case WriteMode:
            writePROM ();
            break;

        case WriteResetVectorMode:
            writeResetVector ();
            break;
        
        case ResetDataProtectionMode:
            resetSDP ();
            break;

        case EnableDataProtectionMode:
            enableSDP ();
            break;
    }

    dumpROM ();
}

void loop () { }
