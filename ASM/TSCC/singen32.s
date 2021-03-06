;==============================
;=
;=  32 bytes sinus table generator
;= for a 16.16 fixedpoint sinus table with 1024 entries
;=
;= by ray//.tSCc.	  2001
;==============================

		section	text
		lea.l	sinus(pc),a0						; 4

		move.w	#512-1,d0						; 4

.gen_loop	move.w	d0,d1		; Approximate the sinus curve using a	; 2
		subi.w	#256,d1		; parabolic graph			; 4
		muls.w	d1,d1							; 2
		subi.l	#$10000,d1	; -sin d0 ~ (d0 - 1) ^ 2 - 1		; 6

		move.l	d1,512*4(a0)	; 3rd & 4th quadrant			; 4
		sub.l	d1,(a0)+	; 1st & 2nd    "			; 2

		dbra	d0,.gen_loop						; 4
										;--
										;32 bytes
		section	bss
sinus		ds.l	1024