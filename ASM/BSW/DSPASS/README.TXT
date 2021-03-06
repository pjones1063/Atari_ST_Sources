Tja, ein DSP-Assembler  zu bekommen,  war gar  nicht  so  einfach.  In  den
Falcon-Buch von Data Becker wird behauptet,  da� ein  DSP-Assembler in  der
Motorola-Mailbox ist,  was  aber  leider  nicht  wahr  ist.  Es  gibt  aber
tats�chlich ein DSP-Assembler, der frei verf�gbar ist.  (Thanks an  Questor
of Inter)

Der Assembler ist  von  Quinn  C.  Jensen.  (  jensenq@npd.novell.com  oder
jensenq@qcj.icon.com) Der Assembler ist als Sourcecode verf�gbar, und  l��t
sich (nach einigen  kleinen Korrekturen)  mit dem  GNU-C-Compiler  auf  den
Atari �bersetzen.

In der Motorola-Mailbox habe ich  ein DSP-Debugger  gefunden. Der  Debugger
l�uft vollst�ndig  in DSP  ab und  l��t sich  somit auf  (fast) jeden  DSP-
System  benutzen.  Die  Kommunikation  mit  den  Anwender  wird  �ber   ein
DSP-Interface (  Host, SCI,  SSI  )  durchgef�hrt.  Leider  l��t  sich  der
Debugger nicht  ohne weiteres  mit den  A56.TTP �bersetzen.  Ich  habe  den
Debugger mit einem selbstgeschriebenen DSP-Assembler �bersetzt, und  hoffe,
das die wichtigsten Funktionen richtig arbeiten.  Folgende Funktionen  habe
ich schon getestet:

-Speicher anzeigen und �ndern
-Register anzeigen und �ndern
-Einzelschritt

Das Debugger-Manual befindet sich in der Datei "MANUAL.MEM". Dort also  bei
Bedarf nachlesen, au�erdem hat der Debugger noch eine HELP-Funktion:

"HEL".

Der Debugger wird  �ber das  Host-Interface gesteuert.  Dies ist  nat�rlich
nicht optimal, da ja  das zu  testende Programm  auch Daten  �ber den  Host
austauschen kann  (wie  z.B.  der  DSP-MOD-Player).  Besser  w�re  es,  das
SCI-Interface zu verwenden. Aber die genialen Entwickler  des Falcons  habe
die SCI-Leitungen nicht  an den  DSP-Connector  angeschlossen.......  Damit
die Sache etwas komfortabler wird, habe ich eine DSP-Shell programmiert:

Mit den Tasten 1, 2, 3, 4 kann man die Pfade und Dateinamen der  ben�tigten
Dateien einstellen.

Mit '3'  wird die  DSP-Source-Datei festgelegt  (  *.A56  ).  Damit  werden
gleichzeitig die Assembler-Output-Datei  ( *.DSP  )  und  das  List-File  (
*.LST )  festgelegt. Die  Output-Datei und  das  List-File  werden  in  den
gleichen Ordner geschrieben, wo sich die Source befindet. Mit '4' wird  ein
CPU-File ( z.B.  *.TOS, *.PRG  )  bestimmt.  Dies  ist  notwendig,  da  ein
DSP-Programm meistens von  ein 68030er  Programm gesteuert  wird. Wenn  das
DSP-Programm nun mit  (G)o gestartet  wird,  wird  !nur!  das  CPU-Programm
aufgerufen. Dieses  CPU-Programm mu�  das DSP-Programm  selbst nachladen  !
Wie das gemacht wird, kann  man in  den DSP-MOD-Player  nachsehen. Mit  "C"
wird das CPU-File "abgeschaltet". Dann wird bei (G)o  das DSP-Programm  von
der Shell  in den  DSP geladen  und  gestartet.  Dann  mu�  man  die  Shell
verlassen und das CPU-File nachtr�glich starten.

Mit "A" wird  der Assembler  gestartet.  Wenn  alles  fehlerfrei  �bersetzt
wurde, erscheint "OK", sonst "Error".

"E" startet den Editor. Am besten  eignet sich  Everest. Everest  speichert
beim Verlassen  alle Einstellungen  automatisch ab.  Deswegen �bergibt  die
Shell auch keine  Command-String an  den Editor.  Die  Fehlermeldungen  des
Assemblers sind in der *.LST-Datei. Wenn Everest  richtig eingestellt  ist,
reicht ein Doppelklick auf die Fehlermeldung um auf  die fehlerhafte  Zeile
der Quelldatei zu kommen.

"D" l�dt das DSP-Programm und startet den Debugger. Der  Debugger wird  mit
ESC beendet.

Die  DSP-Programme  werden  mit  Dsp_ExecBoot  gestartet.  Ich  habe  keine
Ahnung, wie die anderen  DSP-Start-Funktionen funktionieren.  Atari ist  ja
immer sehr bem�ht, die Leute zu informieren..........

Motorola hat da einen besseren Service. Ich habe  das DSP  User's Manual  &
68030 User's Manual umsonst bekommen. Ja, so mu� das sein ! Adresse:

Motorola GmbH Schatzbogen 7 W-8000 M�nchen 82 Tel: 0 89/9 21 03-0  Mailbox:
0 89/9 21 03 111

So, nun noch einige Tip's:

Assembler:

- kein �, �, �, � ! Auch nicht in Kommentarfeld !
- Labels ohne Doppelpunkt am Ende
- Labels d�rfen keine Schl�sselw�rter sein.

Schl�sselw�rter sind alle DSP-Befehle und Register
z.B: m1 add x0,a ;Fehler, da m1 ein Register ist

- Assembler &  Shell verwenden  68030er Befehle,  auf einen  ST werden  die
  Programme also nicht funktionieren

bITmASTER of BSW

