START:	MOVE.W #1,-(SP)  ;READ
	MOVE.W #0,-(SP)
	MOVE.W #0,-(SP)
	MOVE.W #1,-(SP)
	MOVE.W #0,-(SP)
	CLR.L -(SP)
	MOVE.L #BOOT,-(SP)
	MOVE.W #8,-(SP)
	TRAP #14
	ADD.L #20,SP
	TST D0
	BMI ERROR
	RTS

WRITE:	MOVE.W #1,-(SP)   ;EXEC
	MOVE.W #-1,-(SP)
	MOVE.L #-1,-(SP)
	MOVE.L #BOOT,-(SP)
	MOVE.W #18,-(SP)
	TRAP #14
	ADD.L #14,SP
	
	MOVE.W #1,-(SP)
	MOVE.W #0,-(SP)
	MOVE.W #0,-(SP)
	MOVE.W #1,-(SP)
	MOVE.W #0,-(SP)
	CLR.L -(SP)
	MOVE.L #BOOT,-(SP)
	MOVE.W #9,-(SP)
	TRAP #14
	ADD.L #20,SP
	TST D0
	BMI ERROR
	RTS
ERROR:  MOVE.W #$F00,$FF8240
	RTS
	

BOOT:   EQU  $30000

	ORG $30000
	LOAD $30000
EXEC:
BRA ROUT

ORG $3001E
LOAD $3001E

ROUT:
;	MOVE.W #$0,$FF8246
;MIDREZ:	MOVE.W #1,-(SP)
;	MOVE.L #-1,-(SP)
;	MOVE.L #-1,-(SP)
;	MOVE.W #5,-(SP)
;	TRAP #14
;	ADD.L #12,SP 

	CMP.L #0,$42A
	BNE.L FUCK

;	PEA TEXT(PC)  ;STACK ADD. OF TEXT
;	MOVE #9,-(A7) ;PRINT A STRING
;	TRAP #1       ;GEM BDOS
;	ADDQ.L #6,A7  ;TIDY STACK

INVERT: MOVE.L $70,$60000
	LEA VBL(PC),A0
	MOVE.L A0,$70
	CLR.B $4BD
WAIT:	CMP.B #1,READY
	BNE WAIT
	MOVE.L $60000,$70
	MOVE.B #0,READY
	move.w	#0,count
	move.l	#0,$60000
	RTS
***
VBL:	CMP.B #4,$4BD
	BLO RTE  
	CMP.B #1,READY
	BEQ RTE
	MOVE.L A0,-(SP)

	LEA PAL(PC),A0	
	ADD.W COUNT,A0
	MOVE.W (A0),$ffFF8240.w  
	LEA PAL(PC),A0
	ADD.W #30,A0
	SUB.W COUNT,A0
	MOVE.W (A0),$ffFF8246.w
	
DIM1:	ADD.W #2,COUNT  ;to black
	CMP.W #$FFF,(A0)    
	BNE END
	MOVE.W #30,COUNT
	MOVE.B #1,READY
END:	MOVE.L (SP)+,A0
	CLR.B $4BD
RTE:	RTE

TEXT:
DC.B "U stupiiid!  Fuck U sucker! 'dis'"
DC.B "disk belongs to Techwave, 0300-29623"
DC.B 27,113,27,89,33,58
DC.B " So, just... KEEP HACKING PAL'!!!",0
even
PAL:
DC.W $FFF,$777,$EEE,$666,$DDD
DC.W $555,$CCC,$444,$BBB,$333
DC.W $AAA,$222,$999,$111,$888
DC.W $000 	
COUNT:	DC.W 0
READY:	DC.B 0
even


FUCK:	PEA SHIT(PC)
	MOVE.W #9,-(SP)
	TRAP #1
	ADDQ.L #6,SP	
	move.w	#$2700,sr
NOEND:	ADDQ.B #2,$FFFF8240.W
	bra.s	noend

SHIT:
DC.B 27,89,38,32
DC.B "Oups!!! Some shit in res"
DC.B "-vector!!",0
dc.b "HEY!!! DEAR HACKER-SUCKER!!!  FUCK SO MUCH THANKXXX! "
dc.b "THIS BOOT IS LAME BUT I WAS IN A HURRY! "
dc.b "ICE FLUCK NO LAME RULES!  TECHWAVE waves away!"
even
	
