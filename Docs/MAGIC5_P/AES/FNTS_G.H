
typedef	void	(cdecl *UTXT_FN)( WORD x, WORD y, WORD *clip_rect, LONG id, LONG pt, LONG ratio, BYTE *string );

typedef struct _fnts_item
{
	struct	_fnts_item	*next;										/* Zeiger auf den n�chsten Font oder 0L (Ende der Liste) */
	UTXT_FN	display;														/* Zeiger auf die Anzeige-Funktion f�r applikationseigene Fonts */
	LONG		id;															/* ID des Fonts, >= 65536 f�r applikationseigene Fonts */
	WORD		index;														/* Index des Fonts (falls VDI-Font) */
	BYTE		mono;															/* Flag f�r �quidistante Fonts */
	BYTE		outline;														/* Flag f�r Vektorfont */
	WORD		npts;															/* Anzahl der vordefinierten Punkth�hen */
	BYTE		*full_name;													/* Zeiger auf den vollst�ndigen Namen */
	BYTE		*family_name;												/* Zeiger auf den Familiennamen */
	BYTE		*style_name;												/* Zeiger auf den Stilnamen */
	BYTE		*pts;															/* Zeiger auf Feld mit Punkth�hen */
	LONG		reserved[4];												/* reserviert, m�ssen 0 sein */
} FNTS_ITEM;

/* Definitionen f�r <font_flags> bei fnts_create() */

#define	FNTS_BTMP	1													/* Bitmapfonts anzeigen */
#define	FNTS_OUTL	2													/* Vektorfonts anzeigen */
#define	FNTS_MONO	4													/* �quidistante Fonts anzeigen */
#define	FNTS_PROP	8													/* proportionale Fonts anzeigen */

/* Definitionen f�r <dialog_flags> bei fnts_create() */
#define	FNTS_3D		1													/* 3D-Design benutzen */

/* Definitionen f�r <button_flags> bei fnts_open() */
#define	FNTS_SNAME		0x01											/* Checkbox f�r die Namen selektieren */
#define	FNTS_SSTYLE		0x02											/* Checkbox f�r die Stile selektieren */
#define	FNTS_SSIZE		0x04											/* Checkbox f�r die H�he selektieren */
#define	FNTS_SRATIO		0x08											/* Checkbox f�r das Verh�ltnis Breite/H�he selektieren */

#define	FNTS_CHNAME		0x0100										/* Checkbox f�r die Namen anzeigen */
#define	FNTS_CHSTYLE	0x0200										/* Checkbox f�r die Stile anzeigen */
#define	FNTS_CHSIZE		0x0400										/* Checkbox f�r die H�he anzeigen */
#define	FNTS_CHRATIO	0x0800										/* Checkbox f�r das Verh�ltnis Breite/H�he anzeigen */
#define	FNTS_RATIO		0x1000										/* Verh�ltnis Breite/H�he einstellbar */
#define	FNTS_BSET		0x2000										/* Button "setzen" anw�hlbar */
#define	FNTS_BMARK		0x4000										/* Button "markieren" anw�hlbar */

/* Definitionen f�r <button> bei fnts_evnt() */

#define	FNTS_CANCEL	1													/* "Abbruch" wurde angew�hlt */
#define	FNTS_OK		2													/* "OK" wurde gedr�ckt */
#define	FNTS_SET		3													/* "setzen" wurde angew�hlt */
#define	FNTS_MARK	4													/* "markieren" wurde bet�tigt */
#define	FNTS_OPT		5													/* der applikationseigene Button wurde ausgew�hlt */

extern void	*fnts_create( WORD vdi_handle, WORD no_fonts, WORD font_flags, WORD dialog_flags, BYTE *sample, BYTE *opt_button );
extern WORD	fnts_delete( void *fnt_dialog, WORD vdi_handle );
extern WORD	fnts_open( void *fnt_dialog, WORD button_flags, WORD x, WORD y, LONG id, LONG pt, LONG ratio );
extern WORD	fnts_close( void *fnt_dialog );

extern WORD	fnts_get_no_styles( void *fnt_dialog, LONG id );
extern LONG	fnts_get_style( void *fnt_dialog, LONG id, WORD index );
extern WORD	fnts_get_name( void *fnt_dialog, LONG id, BYTE *full_name, BYTE *family_name, BYTE *style_name );
extern WORD	fnts_get_info( void *fnt_dialog, LONG id, WORD *mono, WORD *outline );

extern WORD	fnts_add( void *fnt_dialog, FNTS_ITEM *user_fonts );
extern void	fnts_remove( void *fnt_dialog );

extern WORD	fnts_evnt( void *fnt_dialog, EVNT *events, WORD *button, WORD *check_boxes, LONG *id, LONG *pt, LONG *ratio );
extern WORD	fnts_do( void *fnt_dialog, WORD button_flags, LONG id_in, LONG pt_in, LONG ratio_in, 
							WORD *check_boxes, LONG *id, LONG *pt, LONG *ratio );
