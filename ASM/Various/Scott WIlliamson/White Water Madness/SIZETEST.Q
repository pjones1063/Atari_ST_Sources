
        MOVE.L  #-1,-(SP)       ; REQUEST AMOUNT OF FREE RAM MALLOC
        MOVE    #$48,-(SP)
        TRAP    #1
        ADDQ.L  #4,SP
        JSR     SHOWD0

K       JMP     K

showd0:
  movem.l d0-d7/a0-a6,-(sp)
  move.l d0,d2          ; display long word in d0
  jsr hex2asci
  move.b d0,adr+7
  move.b d1,adr+6
  asr.l #8,d2
  move.l d2,d0
  jsr hex2asci
  move.b d0,adr+5
  move.b d1,adr+4
  asr.l #8,d2
  move.l d2,d0
  jsr hex2asci
  move.b d0,adr+3
  move.b d1,adr+2
  asr.l #8,d2
  move.l d2,d0
  jsr hex2asci
  move.b d0,adr+1
  move.b d1,adr
        MOVE.L  #ADR,-(SP)
        MOVE    #9,-(SP)
        TRAP    #1
        ADD.L   #6,SP
  movem.l (sp)+,d0-d7/a0-a6
  rts

hex2asci:               ; hex in d0 to ascii in d0(low) d1(high)
  move.b d0,d1
  and.b #$f,d0
  cmp.b #$a,d0
  blt lta1
  add.b #'A'-'0'-$a,d0
lta1:
  add.b #'0',d0
  lsr #4,d1
  and.b #$f,d1
  cmp.b #$a,d1
  blt lta2
  add.b #'A'-'0'-$a,d1
lta2:
  add.b #'0',d1
  rts

ADR     DC.B '00000000 ',0

 