/************************************************************************
 * Datei: ApplUtil.c    Zweck: 	N�tzliche Funktionen f�r alle           *
 *                             	GEM.Applikationen                       *
 * Autor: RSB                                                           *
 * Datum: 23.05.90      Grund: 	ShowInfo(), HoleVollName(),Orte() 		*
 *								f�r TOS-Magazin							*
 ************************************************************************/

extern int HoleVollName(char *Pfad, char *VollName, const char *Kommentar);
int Orte(char *Pfad, char *Name, const char *Kommentar);
extern void Clear(int x, int y, int w, int h);
extern void ShowInfo(const char *InfoText);
