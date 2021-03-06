
/*  Example application for ACS    */

/*  "Piano"                        */

/*  14.1.92         Stefan Bachert */

/*(c) 1992 MAXON Computer GmbH     */


#include <tos.h>
#include <acs.h>
#include <piano.h>

/* Lay down Prototypes */

static void ton (void);
static void start (void);
static void play (void);
static Awindow *piano_make (void *not_used);
static int piano_service (Awindow *window, int task, void *in_out);

#include    <piano.ah>

#define MAXSOUND 200

typedef struct {
 int key;
 long time;
 } single;
typedef struct {
 int next;
 single field [MAXSOUND];
 } tape;

/* Sound generator control */
static char sound [] = {
 0x00, 0x10,    /* Set Generator A Frequency */
 0x01, 0x10,
 0x07, 0xf8,    /* Switch rush off           */
 0x0b, 0x00,    /* Tone length sleeve curve  */
 0x0c, 0x30,
 0x08, 0x17,    /* Volume            */
 0x0d, 0x00,    /* Sleeve curve fade */
 0xff, 0x00     /* stop              */
 };

static long timer_200Hz (void)
{
  return *((long *) 0x4BA);  /* fetch 200Hz Timer */
}

static void ton (void)
 
 /* tone of the Frequency carried out (userp1) / 1000 sounds */
   
  
{
  AOBJECT *aob;
  int next;
  long val, timer;
  tape *user;

  aob = (AOBJECT *) ev_object + ev_obnr + 1;
  val = 125000000L / (long) aob-> userp1; /* calculate part value */

  sound [1] = (char) val;                 /* lower Byte           */
  sound [3] = (char) (val >> 8) & 0x0f;   /* upper (Half) Byte    */

  timer = Supexec (timer_200Hz);          /* mark time point      */
  Dosound (sound);
  evnt_timer (80, 0);  /*  x Milli Sec wait (visual return message) */

/* draw */
  user = ev_window-> user;
  next = user-> next ++;
  if (next >= MAXSOUND) return;           /* tape full             */
  user-> field [next]. key = ev_obnr;
  user-> field [next]. time= timer;
}

static void start (void)
 /*
  * Begins at the start with draw, remove !
  */
{
  tape *user;

  user = ev_window-> user;
  user-> next = 0;
}


static void play (void)
 
 /* Plays Band */
  
{ 
  Awindow *window;
  OBJECT *work;
  AOBJECT *aob;
  tape *user;
  long st_time, nxt_time, val;
  int act, key;
  int t, button;

  window = ev_window;
  work = ev_object;

  user = window-> user;

  st_time = Supexec (timer_200Hz) - user-> field-> time; /* skip first time */

  for (act = 0; act < user-> next; act ++) {
 nxt_time = st_time + user-> field [act]. time;
 while (Supexec (timer_200Hz) < nxt_time) {
   graf_mkstate (&t, &t, &button, &t); /* cancel Mouse button */
   if (button != 0) break; 
 };                                    /* wait                */

 key = user-> field [act]. key;
 aob = (AOBJECT *) work + key + 1;
 val = 125000000L / (long) aob-> userp1; /* calculate part value */
 sound [1] = (char) val ;                /* lower Byte           */
 sound [3] = (char) (val >> 8) & 0x0f;   /* upper (Half) Byte    */

 (window-> obchange) (window, key, work [key]. ob_state | SELECTED);
 Dosound (sound);
 evnt_timer (80, 0);  /*  x Milli Sec wait (visual return message) */
 (window-> obchange) (window, key, work [key]. ob_state & ~SELECTED);

 graf_mkstate (&t, &t, &button, &t); /* cancel Mouse button*/
 if (button != 0) break; 
  };
}


static Awindow *piano_make (void *not_used)
    
    /*  Generate Piano Window */
     
{
  Awindow *wi;
  tape *user;

  wi = Awi_create (&PIANO);
  if (wi == NULL) return NULL;

  user =
  wi-> user = Ax_malloc (sizeof (tape)); /* Lay down Data structure */
  if (user == NULL) return NULL;   /* Error passed */

  user-> next = 0;        /* Initialise */

  if (application) {
 (wi-> open) (wi);      /* open similiar */
  };
  return wi;
}


static int piano_service (Awindow *window, int task, void *in_out)
{
  switch (task) {
 case AS_TERM:               /* Release Window     */
   Ax_free (window-> user);  /* Release Structure  */
   Awi_delete (window); break;
 default:
   return FAIL;
  };
  return TRUE;
}


static char oldconterm; /* for original value */

static long off_click (void)
{
  oldconterm = *((char *) 0x484);
  *((char *) 0x484) &= ~3;  /* no click and no Key repetition */
  return 0L;
}


static long old_click (void)
{
  *((char *) 0x484) = oldconterm;
  return 0L;
}


void ACSterm (void)
{
  Supexec (old_click);     /* old state */
}


int ACSinit (void)
    
    /*  Double click on NEW produces a new Window */
     
{
  Awindow *window;

  Supexec (off_click);     /* switch click off */

  if (application) {
 window = Awi_root ();                 /* root window */

 if (window == NULL) return FAIL;      /* set down NEW Icon */
 (window-> service) (window, AS_NEWCALL, &PIANO. create);

 window = &PIANO;
 (window-> create) (NULL);             /* Generate a Window immediately */
  };

  return OK;
}
