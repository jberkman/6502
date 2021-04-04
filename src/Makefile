AS = vasm6502_oldstyle
ASFLAGS = -Fbin -dotdir

BINS = \
	blink.bin \
	hello.bin

all: ${BINS}

%.bin: %.s
	${AS} ${ASFLAGS} $< -o $@

.SUFFIXES:
.SUFFIXES: .bin
