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

ORG $3001c
LOAD $3001c

ROUT:	MOVE.W #$003,$FFFF8240.W
	MOVE.W #$777,$FFFF825E.W
	MOVE.B #$2,$484.W

	PEA TEXT(PC)  ;STACK ADD. OF TEXT
	MOVE #9,-(SP) ;PRINT A STRING
	TRAP #1       ;GEM BDOS
	ADDQ.L #6,SP  ;TIDY STACK

	CMP.L #0,$42A
	BNE FUCK
;	CMP.L #$E09520,$84
;	BNE FUCK
;	CMP.L #$E00940,$B4
;	BNE FUCK
;	CMP.L #$E0093A,$B8
	BRA.S KEY

FUCK:	PEA SHIT(PC)
	MOVE.W #9,-(SP)
	TRAP #1
	ADDQ.L #6,SP	
NOEND:	ADDQ.B #1,$FFFF8240.W
	ADDQ.L #1,CNT
	CMP.L #$AFFFF,CNT
	BNE.S NOEND
	MOVE.L #0,$42A
;	MOVE.L #$E09520,$84
;	MOVE.L #$E00940,$B4
;	MOVE.L #$E0093A,$B8
	BRA.L ROUT
	
KEY:	MOVE.W #7,-(SP)
	TRAP #1	
	ADDQ.L #2,SP
	CMP.B #$D,D0
	BEQ.S RTS
	
MIDREZ:	MOVE.W #$777,$FFFF8246.W
	MOVE.W #1,-(SP)
	MOVE.L #-1,-(SP)
	MOVE.L #-1,-(SP)
	MOVE.W #5,-(SP)
	TRAP #14
	ADD.L #12,SP 
RTS:	RTS

TEXT:
DC.B 27,69,13,10
DC.B " Tobias Nilsson 'Techwave'"
DC.B " : 0300-29623"		
DC.B 27,89,34,40
DC.B "<CR> to suck for nothing,"
DC.B 27,89,35,36
DC.B "Or fuck any other key for "
DC.B "Midrez!",0
SHIT:
DC.B 27,89,38,32
DC.B "Hey!!! Some shit in res"
DC.B "-vector!!!",0
CNT: DC.L 0
