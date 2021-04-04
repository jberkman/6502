PORTB = $6000
PORTA = $6001
DDRB  = $6002
DDRA  = $6003

  .org $8000
reset:
  lda #%11111111 ; Set all pins on port B to output
  sta DDRB

  lda #$50
  sta PORTB

loop:
  ror
  sta $6000

  jmp loop

  .org $fffc
  .word reset
  .word $0000
