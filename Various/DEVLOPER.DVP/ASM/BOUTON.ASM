;
; D�mo evnt_button     (Gestion �v�nements)
; Assembler        BOUTON.ASM
;

               .INCLUDE 'GEM_INEX.ASM'

gemdos         = 1
cconws         = 9

               .TEXT 

main:          jsr        gem_init

               pea        clrscr        ;Effacer l'�cran
               move.w     #cconws,-(sp)
               trap       #gemdos
               addq.l     #6,sp

; Activer le mode transparent

               move.w     #32,contrl    ;Opcode
               clr.w      contrl+2
               clr.w      contrl+4
               move.w     #1,contrl+6
               move.w     #1,contrl+8
               move.w     handle,contrl+12

               move.w     #2,intin      ;2 = Transparent

               jsr        vdi

; Le pointeur souris actif apr�s le chargement est l'abeille
; Cela g�ne notre programme. Nous passons donc
; � la fl�che normale, par la fonction AES graf_mouse.

               move.w     #78,control
               move.w     #1,control+2
               move.w     #1,control+4
               move.w     #1,control+6
               clr.w      control+8

               clr.w      int_in        ;0 pour la fl�che
               clr.l      addr_in       ;pour nous: nul dummy (factice)

               jsr        aes

loop:          
; evnt_multi - Aufruf

               move.w     #21,control   ;AES, donc contro(!)l
               move.w     #3,control+2
               move.w     #5,control+4
               clr.w      control+6
               clr.w      control+8

               move.w     #2,int_in     ;Enregistrer le double-clic maximal
               move.w     #1,int_in+2   ;Consulter le bouton gauche
               move.w     #1,int_in+4   ;�v�nement: bouton souris enfonc�

               jsr        aes

               tst.w      int_out+8     ;�tat des touches sp�ciales
               bne        ende          ;Fin, si touche ou bouton appuy�

               cmpi.w     #1,int_out    ;Combien de clics?
               bne.s      l
               move.b     #'x',d0       ;Un, alors �crire un x minuscule
               bra.s      ll
l:             move.b     #'X',d0       ;Sinon, double clic et X majuscule
ll:            

; v_gtext appeler

               move.w     #8,contrl
               move.w     #1,contrl+2
               clr.w      contrl+4
               clr.w      contrl+8
               move.w     handle,contrl+12

               move.w     int_out+2,ptsin         ;Coordonn�es par evnt_button
               move.w     int_out+4,ptsin+2

               move.b     d0,affichage  ;Convertir caract�re en cha�ne
               lea.l      affichage,a0
               bsr        fix_text      ;�crirer cha�ne dans tableau intin

               jsr        vdi

               bra        loop

ende:          jsr        gem_exit

               clr.w      -(sp)
               trap       #gemdos


fix_text:      
; Sous-programme, qui �crit une cha�ne (passer l'adresse
; � a0) dans le tableau intin-Array, d�termine sa longueur
; et la stocke dans contrl[3]

               clr.w      d0            ;Longueur
               clr.w      d1            ;Registre d'aide
               lea.l      intin,a1

fix_loop:      move.b     (a0)+,d1      ;Lire un octet dans cha�ne cible
               tst.b      d1            ;Fin de cha�ne?
               beq.s      fix_end
               move.w     d1,(a1)+      ;Non, alors �crire dans tableau
               addq.w     #1,d0         ;intin comme mot
               bra.s      fix_loop

fix_end:       move.w     d0,contrl+6   ;Fixer la longueur
               rts        

               .DATA 

affichage:     .DC.w 0

clrscr:        .DC.b 27,'E',0

               .END 
