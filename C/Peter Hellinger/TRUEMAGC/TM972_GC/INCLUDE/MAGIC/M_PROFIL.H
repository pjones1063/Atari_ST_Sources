/*
   Modul: M_PROFIL (Headerdatei)
   
   geschrieben von Thomas K�nneth
   Friedrich-Neuper-Str. 2b
   90552 R�thenbach
   
   Diese Datei ist Bestandteil der C-Version von Magic 4.0 von
   Peter Hellinger und Thomas K�nneth und darf nur gem�� den
   Nutzungsbedingungen von Magic f�r C verwendet werden
   
   Alle Rechte beim Autor
*/

#ifndef _M_PROFIL_
#define _M_PROFIL_

#ifndef _MAGICTYP_
#include <magic\m_types.h>
#endif

BOOLEAN ReadProfile(char *name);
void FreeProfile(void);
BOOLEAN GetValue(char *token,long *val);
BOOLEAN GetBoolean(char *token,BOOLEAN *val);
BOOLEAN GetString(char *token,char *val);
BOOLEAN GetSV(char *token, int num, long val[]);

#endif