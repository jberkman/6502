AS = vasm6502_oldstyle
ASFLAGS = -Fbin -dotdir
MINIPRO = minipro
MINIPROFLAGS = -p AT28C256

BINS = \
	blink.bin \
	hello.bin

all: ${BINS}

%.bin: %.s
	${AS} ${ASFLAGS} $< -o $@

install-%: %.bin
	${MINIPRO} ${MINIPROFLAGS} -w $<

.SUFFIXES:
.SUFFIXES: .bin
