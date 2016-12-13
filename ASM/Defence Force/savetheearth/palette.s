
 
; 
; Transforms a PC like RGB 24 bits colors
; in a STE color.
; \1 = red
; \2 = green
; \3 = blue
MAKERGB macro
var_r set (((\1)&255)>>4)
var_g set (((\2)&255)>>4)
var_b set (((\3)&255)>>4)
var_r set ((var_r>>1)+((var_r&1)<<3))
var_g set ((var_g>>1)+((var_g&1)<<3))
var_b set ((var_b>>1)+((var_b&1)<<3))
 dc.w (((var_r)&15)<<8)+(((var_g)&15)<<4)+((var_b)&15)
 endm
 
; 
; Transforms a STF like colors in a STE color.
; \1 = red
; \2 = green
; \3 = blue
MAKECOLOR macro
var_r set (\1)
var_g set (\2)
var_b set (\3)
var_r set ((var_r>>1)+((var_r&1)<<3))
var_g set ((var_g>>1)+((var_g&1)<<3))
var_b set ((var_b>>1)+((var_b&1)<<3))
 dc.w (((var_r)&15)<<8)+(((var_g)&15)<<4)+((var_b)&15)
 endm

 
 SECTION TEXT

WaitEndOfFade 
loop_wait_end_of_fade
 bsr WaitVbl
_AdrEndOfFadeRoutine=*+2
 jsr DummyRoutine
 
 move.b palette_fade_counter,d0
 bne.s loop_wait_end_of_fade
 bsr WaitVbl
 rts
 
; a0=source palette 
PaletteSet
 movem.l a0/a1,-(sp)
 ; Need that one to avoid the issue with the last grey palette generated by the subtitles...
 lea palette_vbl_fade,a0	; 12/3
 lea $ffff8240.w,a1	    	; 8/2
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 move.l (a0)+,(a1)+    		; 20/5
 movem.l (sp)+,a0/a1
 rts
  
  
;
; a0 -> start palette
; a1 -> end palette
; a2 -> destination palettes
; d7 -> number of colors
;
ComputeFullGradient 
 move.l a2,-(sp)
 bsr ComputeGradient
 
 move.l (sp)+,a0
.loop 
 bsr ApplyGradient
 tst palette_fade_counter
 bne.s .loop
 rts
 
;
; a0 -> start palette
; a1 -> end palette
; d7 -> number of colors
;
ComputeGradient
 ;
 ; Compute fixed point increment values
 ;
 lea palette_source_rgb,a2  	; Source palette and increments stored as separated components
 lea table_to_stf,a3	; Conversion table from STE encoding to linear
 subq.l #1,d7
 move.w d7,palette_fade_color_count
.loop_color
 ; Read source and destination colors
 move.w (a0)+,d0			
 move.w (a1)+,d1
 
 ; Blue component
 move d0,d2
 and #%1111,d2
 moveq #0,d5
 move.b (a3,d2.w),d5	; Source BLUE STE to linear conversion
 swap d5
 
 move d1,d2
 and #%1111,d2
 moveq #0,d6
 move.b (a3,d2.w),d6	; Destination BLUE STE to linear conversion
 swap d6
 sub.l d5,d6
 asr.l #4,d6
 move.l d6,(a2)+		; Store increment
 move.l d5,(a2)+		; Store base color

 ; Green component
 lsr #4,d0
 move d0,d2
 and.w #%1111,d2
 moveq #0,d5
 move.b (a3,d2.w),d5
 swap d5
 
 lsr #4,d1
 move d1,d2
 and #%1111,d2
 moveq #0,d6
 move.b (a3,d2.w),d6
 swap d6
 sub.l d5,d6
 asr.l #4,d6
 move.l d6,(a2)+		; Store increment
 move.l d5,(a2)+		; Store base color

 ; Red component
 lsr #4,d0
 move d0,d2
 and #%1111,d2
 moveq #0,d5
 move.b (a3,d2.w),d5
 swap d5
 
 lsr #4,d1
 move d1,d2
 and #%1111,d2
 moveq #0,d6
 move.b (a3,d2.w),d6
 swap d6
 sub.l d5,d6
 asr.l #4,d6
 move.l d6,(a2)+		; Store increment
 move.l d5,(a2)+		; Store base color

 dbra d7,.loop_color
 
 move.b #16,palette_fade_counter
 rts
 
   
;
; Then do the fade
; a0=adress of destination palette
ApplyGradient
 move.b palette_fade_counter,d0
 beq.s .done
.continue_gradient
 subq #1,d0
 move.b d0,palette_fade_counter
 
 lea palette_source_rgb,a1
 lea table_to_ste,a2
 
 move.w palette_fade_color_count,d7
.loop_color
 move.l (a1)+,d0    ; Blue increment
 add.l (a1),d0		; + Blue base color
 move.l d0,(a1)+	; Store new Blue

 move.l (a1)+,d1    ; Green increment
 add.l (a1),d1		; + Green base color
 move.l d1,(a1)+	; Store new Green

 move.l (a1)+,d2    ; Red increment
 add.l (a1),d2		; + Red base color
 move.l d2,(a1)+	; Store new Red

 swap d0
 swap d1
 swap d2

 move.b  0(a2,d2.w),(a0)+ ; Red to STE
 move.b 16(a2,d1.w),d2    ; Green to STE
 or.b    0(a2,d0.w),d2	; Blue to STE
 move.b d2,(a0)+		; Write final color
 
 dbra d7,.loop_color
.done 
 rts


; Input: 
; d0 - start RGB values (eg: 255,255,255) 
; d1 - end RGB values (eg: 0,0,0)
; a2 - destination buffer (eg: buffer_background_color) 
; d7 - number of steps (ie: number of colors)
;
; Output:
; All registers are restored to the initial value, excepted
; a2 which points to the end of the buffer. This way things 
; can be daisy chained nicely.
CreateBackgroundGradient
 movem.l d0-d7/a0/a1/a3/a4,-(sp)
 
 lea palette_start_colors,a0
 movep.l d0,1(a0)
 movem.w 2(a0),d3/d4/d5		; Source red/green/blue
 
 lea palette_delta_colors,a1
 movep.l d1,1(a1)
 moveq #0,d1
 movep.l d1,0(a1)			; Need to clear the low byte to clean previous computation results
   
 sub.w d3,2(a1)				; Delta red
 sub.w d4,4(a1)				; Delta green
 sub.w d5,6(a1)				; Delta blue
	 
 lea table_to_ste,a3		; +0: normal table, +16: shifted by 4
  
 moveq #0,d6	; current index
.loop
 movem.w 2(a1),d0/d1/d2		; red/green/blue deltas

 muls d6,d0
 divs d7,d0
 add d3,d0	; Red start

 muls d6,d1
 divs d7,d1
 add d4,d1	; Green start

 muls d6,d2
 divs d7,d2
 add d5,d2	; Blue start

 lsr.b #4,d2
 lsr.b #4,d1
 lsr.b #4,d0
  
 move.b  0(a3,d0.w),(a2)+ 	; Red to STE
 move.b 16(a3,d1.w),d0    	; Green to STE
 or.b    0(a3,d2.w),d0		; Blue to STE
 move.b d0,(a2)+			; Write final color
  
 addq #1,d6
 
 cmp d6,d7
 bne.s .loop
 
 movem.l (sp)+,d0-d7/a0/a1/a3/a4
 rts 
 
 
; a0 - Source buffer (r,g,b components)
; a1 - Target buffer (16 bits colors, STE interleaved format)
; d7 - Number of colors
RawToSt 
 ;
 ; First generate the 16 bits picture from the 24 bits
 ;
 lea table_to_ste,a2

 ;move.w #170*8,d7
.loop
 move.b (a0)+,d0	; R 111xxxxx
 move.b (a0)+,d1	; G
 move.b (a0)+,d2	; B

	 ; Let's get it a bit darker for the visibility
	 ;lsr.b #1,d0
	 ;lsr.b #1,d1
	 ;lsr.b #1,d2
 
 lsr.b #4,d0
 and #15,d0
 move.b (a2,d0),d0

 lsr.b #4,d1
 and.w #15,d1
 move.b (a2,d1),d1

 lsr.b #4,d2
 and.w #15,d2
 move.b (a2,d2),d2

 lsl.w #8,d0
 lsl.w #4,d1
 or.w d1,d0
 or.w d2,d0

 move d0,(a1)+

 dbra d7,.loop
 rts

 	 
 even  
 
 SECTION DATA

table_to_stf
 dc.b 0
 dc.b 2
 dc.b 4
 dc.b 6
 dc.b 8
 dc.b 10
 dc.b 12
 dc.b 14
 dc.b 1
 dc.b 3
 dc.b 5
 dc.b 7
 dc.b 9
 dc.b 11
 dc.b 13
 dc.b 15

table_to_ste
 dc.b 0
 dc.b 8
 dc.b 1
 dc.b 9
 dc.b 2
 dc.b 10
 dc.b 3
 dc.b 11
 dc.b 4
 dc.b 12
 dc.b 5
 dc.b 13
 dc.b 6
 dc.b 14
 dc.b 7
 dc.b 15

table_to_ste_shifted
 dc.b (0<<4)
 dc.b (8<<4)
 dc.b (1<<4)
 dc.b (9<<4)
 dc.b (2<<4)
 dc.b (10<<4)
 dc.b (3<<4)
 dc.b (11<<4)
 dc.b (4<<4)
 dc.b (12<<4)
 dc.b (5<<4)
 dc.b (13<<4)
 dc.b (6<<4)
 dc.b (14<<4)
 dc.b (7<<4)
 dc.b (15<<4)
 
GreyPalette
var set 0
 REPT 16
 MAKECOLOR var,var,var
var set var+1 
 ENDR
 
 even

 SECTION BSS
 	 
palette_vbl_fade			ds.w 16

 even
 
palette_source_rgb	 		ds.l 16*6		; RGB components of palettes about to be faded
palette_fade_color_count 	ds.w 1			; How many colors do we want to fade (max 256)
palette_fade_counter		ds.b 1

 even
 
palette_start_colors		ds.w 3+1		; Signed value, start component values
palette_delta_colors		ds.w 3+1		; Signed value, difference between end and start component values
 
 even
 

 