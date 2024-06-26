/* Copyright (c) 1988,1989 by Sozobon, Limited.  Author: Johann Ruegg
 *           (c) 1990 - present by H. Robbers.   ANSI upgrade.
 *
 * This file is part of AHCC.
 *
 * AHCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * AHCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AHCC; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *  AENDERUNGEN AN DIESER DATEI: Die Fehlermeldungen des Compilers werden
 *      in eine Datei geschrieben, die den Namen des C-Quelltextes mit der
 *      Endung "ERR" erhaelt. Dadurch ist es in jeder Version des
	 *      Betriebs-Systems moeglich, die Fehlermeldungen in eine Datei zu
 *      bekommen. Die Aenderungen betreffen die Funktionen
 *      main(), error...(), warn...(), fatal...() und optnl()
 *  	    -- Holger --
 *
 *	main.c
 *
 *	Main routine, options, error handling.
 */

#define WAIT 0

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tos.h>
#include <ext.h>
#include <setjmp.h>
#include "common/mallocs.h"
#include "common/amem.h"
#include "common/dict.h"

#if BIP_CC
#include "shell/shell.h"
#include "common/pdb.h"
#endif

#include "param.h"
#include "ahcc.h"
#include "tok.h"
#include "pre.h"
#include "body.h"
#include "gsub.h"
#include "out.h"
#include "opt.h"


#define debugO (G.xflags['o'-'a'])
#define debugN (G.xflags['n'-'a'])
#define debugD (G.xflags['d'-'a'])
#define debugZ (G.xflags['z'-'a'])

enum { dflags = 'z'-'a' };

const char * pluralis(short);

global long	line_no, n_line_no;
global COMMON G;
global CC_PHASE phase;

#if ! BIP_CC
static
const char
	corq   [] = "C or Q ? ",
	wacht  [] = "Press any key.\n";
bool MiNT, MagX;
bool get_cookie(long cookie, long *value);
Cstr pluralis(short n);
#else
global
STATS stats = {0};
#endif

static
CP input;
static
short succ, breakin;

global
AHCOUT *bugf = stdout;

#define MAXPREDEF	128

global
DEFS defines[MAXPREDEF];		/* NN..BB.. There are also in init_cc() for BIP_CC */

#define BRKVAL 20

global
Cstr ahcc_version =
#if FOR_A || LL
                    "v8"
#else
                    CVERSION
#endif
      ;

static
const char
	Version[] =
#ifdef PRGNAME
		PRGNAME
#else
		"AHCC"
#endif
			" %s (c) 1988 by Sozobon, 1990 - present by H.Robbers Amsterdam\n";

static
bool toomanyerr(void)
{
	if (G.nmerrors > G.e_max_errors)
	{
		if (!G.egiven)
			console("more than %d error%s\n", G.e_max_errors, pluralis(G.e_max_errors)),
			G.egiven = true;
		return true;
	}
	return false;
}

static
bool toomanywarn(void)
{
	if (G.nmwarns > G.f_max_warnings)
	{
		if (!G.wgiven)
			console("more than %d warning%s\n", G.f_max_warnings, pluralis(G.f_max_warnings)),
			G.wgiven = true;
		return true;
	}
	return false;
}

/* only called if C_DEBUG, but left in because also usable w/o C_DEBUG */
global
void send_incname(short inclvl, Cstr incname)
{
	Cstr s = incname+strlen(incname);	/* display short name */
	while (s > incname)
		if (*s eq bslash or *s eq fslash)
		{
			s++;
			break;
		}
		else
			s--;
#if 0
  #if BIP_CC
    #if C_DEBUG
    	console("%d>[%d]%s\n", inclvl, G.inctab->p.fileno, s);
    #else
    	console("%d>%s\n", inclvl, incname);
    #endif
  #else
  	console(inclvl ? "\t" : "\n");
    #if C_DEBUG
      console("[%d]%s ", G.inctab->p.fileno, s);
    #else
      console("%s ", s);
    #endif
  #endif
#endif
}

global
void adddef(Cstr ns)		/* adddef subdef & dodefs 3'91 v1.2 */
{
	if (*ns eq 0)		/* 09'16 HR v5.8 */
		return;

	/* 10'12 HR 4.14 cannot keep static Cstr ns */
	if (G.npred >= MAXPREDEF)
		error("More than %d -D's", MAXPREDEF);
	else
	{
		Wstr as, s = CC_xmalloc(strlen(ns)+1, AH_PREDEF, CC_ranout);
		if (s)
		{
			strcpy(s, ns);		/* 01'17 HR v5.5: replaced strncpy by strcpy; misread strncpy docu */
			as = strchr(s,'=');

			if (as)
				*as++ = 0;

			defines[G.npred].dname = s;
			defines[G.npred].dval  = as;
			G.npred++;
			defines[G.npred].dname = nil;
			defines[G.npred].dval  = nil;
		}
	}
}

global
void freedefs(void)
{
	short i;
	struct def *pt;

	pt = defines;
	for (i = 0; i < G.npred; i++, pt++)
		CC_xfree(pt->dname);
}

static
void dodefs(void)
{
	short i;
	struct def *pt;

	/*
	 * Define the "built-in" macros
	 */
	pt = defines;
	for (i = 0; i < G.npred; i++, pt++)
	{
		Cstr s = pt->dname, as = pt->dval;
		if (!as)
			as = "1";
		if (G.v_Cverbosity > 3)
			console("def: '%s','%s'\n", s, as);

		optdef(s, as);
	}
}

static
void optnl(void)
{
	if (bugf eq stdout and G.anydebug)
		console("\n"), breakin++;
}

static
jmp_buf cc_jump;
static
char *abort_msg = "**** compiler aborted, insufficient memory. ****\n";
static
short xjmp;

global
XA_run_out CC_ranout
{
	if (phase > COMPILING)
	{
		console("Internal error: CC_ranout wrong phase %d, key %d", phase, xkey);
		bios(2,2);
		exit(9999);
	}

	if (base->flags & XA_LOCAL)
		XA_free_all(base, -1, -1);

	G.nmerrors++;		/* prevents output */
	longjmp(cc_jump, 1);
	return 1;			/* dummy: not reached ever */
}

global
char trtime[10],
	        trdate[24];		/* for use in pre.c */

static
short do_file(void)
{
	long clo=clock();
	XA_report cpunit;
	VpV asmc_end, opc_end, setup_end;

	union
	{
		long lt;
		struct
		{
			ubits y : 7;
			ubits m : 4;
			ubits d : 5;
			ubits h : 5;
			ubits mn : 6;
			ubits s : 5;
		} b;
	} tij;

	extern XP *deflist, curtok;		/* from  PRE  */
	short i;

	tij.lt = Gettime();
	i = tij.b.y + 1980;

	sprintf(trtime, "%02u:%02u:%02u", tij.b.h, tij.b.mn, tij.b.s);
	sprintf(trdate, "%04u-%02u-%02u", i,       tij.b.m,  tij.b.d);

	breakin = 0;

#if !BIP_CC
	MagX  = get_cookie('MagX',nil);
	MiNT  = get_cookie('MiNT',nil);

#endif

	init_cache();
	tok_init();

	deflist = init_symtab(AH_NEW_DEFLIST);
	symtab = init_symtab(AH_NEW_SYMTAB);
	curtok = allocXn(1);

	dodefs();							/* 3'91 v1.2 */

	line_no = n_line_no = 1;
	cur_LEX = input->text;
	xjmp = setjmp(cc_jump);

#if DEBUG
	if (xjmp eq 1)
	{
		console("----> cc_abort\n");
		goto cc_abort;
	}
#else
	if (xjmp eq 1) goto cc_abort;
#endif

#if C_DEBUG
	pstk = 0;
#endif

#if BIP_ASM
	if (G.xlang eq 's')
	{
		do_S();				/* assemble a S file */
	}
	else
#endif
	{
		if (!create_areas())
			error("[1]Not enough memory for output");
		else
			do_C();				/* compile a C or A file */
	}

	write_literals();

	G.eof = true;			/* 12.14 v5.2 for messages (dont use G.inctab) */

cc_abort:
	if (deflist)
	{
		for (i = 0; i < numhash; i++)
		{
			freeXn(deflist[i]);
			deflist[i] = nil;
		}
		CC_xfree(deflist);
		deflist = nil;
	}

	freeXn(G.holdtok);
	G.holdtok = nil;
	freeXn(G.expanding);
	G.expanding = nil;

	if (symtab)
	{
		for (i = 0; i < numhash; i++)
		{
			freeTn(symtab[i]);
			symtab[i] = nil;
		}
		CC_xfree(symtab);
		symtab = nil;
	}


	freeTn(G.optab);
	G.optab = nil;

	freeTn(G.casttab);
	G.casttab = nil;

	freeTn(G.tagtab);
	G.tagtab = nil;

	freeincs();
	free_cache_unit(input);

#if !BIP_CC || !__COLDFIRE__
	free_dictionary();
#endif

#if  0 /* NODESTATS */
	console("unique id's: %ld\n", dictionary_names);
	console("heap's     : %ld\n", heap_names);
#endif

	freenodespace(XNr);
	freenodespace(TLr);
	freenodespace(EXr);
	freenodespace(VNr);

	while (G.scope)
	{
		SCP nx = G.scope->outer;
		CC_xfree(G.scope);
		G.scope = nx;
	}

	if (G.v_Cverbosity > 1)
		console("\n");

	clo = clock()-clo;	/* wide measurement */
	G.clockopt = clock();

#if BIP_ASM
	if (G.lang eq 's')
		end_S();
	else
#endif
		end_C();		/* calls optimizer */

#if BIP_ASM
	asmc_end();			/* 12'09 HR: Also frees C embedded asm setup */
#endif
	freenodespace(INr);
	freenodespace(BKr);

	destroy_areas();	/* 12'09 HR: moved to here (asm was leaking) */

	G.clockopt = clock()-G.clockopt;;

	if (G.nmmessages)
		console("%d message%s\n", G.nmmessages, pluralis(G.nmmessages));
	if (G.nmwarns and G.nmwarns <= G.f_max_warnings)
		console("%d warning%s\n", G.nmwarns, pluralis(G.nmwarns));
	if (G.nmerrors and G.nmerrors <= G.e_max_errors)
		console("%d error%s\n", G.nmerrors, pluralis(G.nmerrors));

	{
		short fclo;

		if (G.an_no_O)
		{
			fclo = clo/2;
			console("clocked: %d.%02d\n", fclo/100, fclo%100);
			if (G.nmerrors)
				console("processed %ld bytes.\n", G.C_bytes);
			elif (fclo)
				console("compiled %ld bytes; thats %ld Bps.\n", G.C_bytes, (G.C_bytes*100)/fclo);
			else
				console("compiled %ld bytes.\n", G.C_bytes);
		othw
			short pclo;
			fclo = clo/2;
			if (G.nmerrors)
			{
				if (fclo)
					console("clocked %d.%02d\n",fclo/100,fclo%100);
				console("processed %ld bytes.\n",G.C_bytes);
			othw
				G.clockopt  -= G.asm_clock;
				G.asm_clock /= 2;
				pclo = G.clockopt/2;

				pclo += G.asm_clock;
				if (pclo and fclo)
					console("compiled %ld bytes in (%d.%02d + %d.%02d) seconds",
						G.C_bytes, fclo/100,fclo%100,
						           pclo/100,pclo%100);
				elif (fclo)
					console("compiled %ld bytes in %d.%02d seconds",
						G.C_bytes, fclo/100,fclo%100);
				elif (pclo)
					console("compiled %ld bytes in %d.%02d seconds",
						G.C_bytes, pclo/100,pclo%100);
				else
					console("compiled %ld bytes in no time\n",
						G.C_bytes);

				fclo += pclo;
				if (fclo)
					console("; %ld Bps\n", (G.C_bytes*100)/fclo);
			}
		}
	}

#if NODESTATS
	if (G.al_list_stats)
		nodecnts();
#endif

	if (xjmp)
	{
		console(abort_msg);
#if BIP_ASM
		asmc_end();
#endif
		opc_end();
		CC_xfree_all(-1);
		CC_ffree_all(-1);
		NS_xfree_all(-1);
	}

	free_cache();

	if (G.nmerrors)
		return G.nmerrors;
	if (G.nmwarns)
		return -G.nmwarns;			/* '-' tbv SHELL */

	return 0;
}

global
XA_report cpunit
{
	if (!unit)
		printf("Leak: nil\n");
	else
	{
		XA_key key = unit->key;
		if (G.ac_cache_headers)
			if (key eq AH_OPEN_C or key eq AH_OPEN_X)
				return;
		/* see common/mallocs.h */
		if (key < CC_LEAK_BEGIN or key > CC_LEAK_END)
			return;

		console("Leak: %3d size %5ld %lx block %5ld %lx %s\n",
			key, unit->size, unit->area, blk->size, blk, base->name);
	}
}

global
void subdef(Cstr s)
{
	short i;

	for (i = 0; i < G.npred; i++)
		if (SCMP(0, s, defines[i].dname) eq 0)
		{
			CC_xfree(defines[i].dname);		/* 01'18 HR: v6 */
			while (i < G.npred)
			{
				defines[i] = defines[i+1];
				i++;
			}
			G.npred--;
			return;
		}
}

static
char *ON(bool opt)
{
	return opt ? "ON" : "OFF";
}

static
bool on_off(Cstr s, bool *b)
{
	if (*s eq '-') { s++; *b = false; return true; }
	if (*s eq '+') { s++; *b = true ; return true; }		/* goto next - */
	*b = true;
	return false;		/* next letter */
}

#define ON_OFF(x) if (on_off(s, &(x))) return; else continue

static
void help_options(char c)
{
	if (c)
		console("\nwarning: unknown option: %c\tthey are:\n\t(followed by current state)\n", c);
	console("AHCC specific options:\n");
	console("-*g    warn goto's                         (%s)\n", ON(G.ag_nogoto));
	console("-*b    no branch reversals                 (%s)\n", ON(G.ab_no_branch_reversals));
	console("-*i    default int is 32 bits              (%s)\n", ON(G.ai_int32));
	console("-*w    default Xn is long                  (%s)\n", ON(G.aw_Xnl));
	console("-*u    Assembly starts in .super mode      (%s)\n", ON(G.au_supervisor));
	console("\nPure C compatible options:\n");
	console("-c     allow nested comments               (%s)\n", ON(G.c_nested_comments));
	console("-dX..X define macro; optional =value\n");
	console("-eN    maximum no of erors                 (%d)\n",    G.e_max_errors);
	console("-fN    maximum no of warnings              (%d)\n",    G.f_max_warnings);
	console("-h     cdecl calling                       (%s)\n", ON(G.h_cdecl_calling));
	console("-iX..X include directory\n");
	console("-k     default char is unsigned            (%s)\n", ON(G.k_char_is_unsigned));
	console("-l     maximum identifier length           (%d)\n",    G.l_identifier_max_length);
	console("-uX..X undefine macro\n");
	console("-v     verbosity                           (%d)\n",    G.v_Cverbosity);
	console("-x     prepend underline to identiers      (%s)\n", ON(G.x_add_underline));
#if FLOAT
	console("-2     generate for MC68020+               (%s)\n", ON(G.i2_68020));
	console("-3     generate for MC68030+               (%s)\n", ON(G.i2_68030));
	console("-4     generate for MC68040+               (%s)\n", ON(G.i2_68040));
	console("-6     generate for MC68060+               (%s)\n", ON(G.i2_68060));
	console("-8     generate directly for MC68881/2     (%s)\n", ON(G.use_FPU));
#endif
#if COLDFIRE
	console("-7     Coldfire                            (%s)\n", ON(G.Coldfire));
	console("-27    Coldfire, also runnable on 68020 (%s,%s)\n", ON(G.Coldfire),ON(G.i2_68020));
#endif
#if C_DEBUG
#if ! BIP_CC
	if (!MiNT and !MagX)
	{
		console(wacht);
		bios(2,2);
	}
#endif
	console("debug options:\n");
	console("-*t    make listing of internal token names and values\n");
	console("-*e    suppress extracodes (ahcc_rt.h)\n");
	console("-*a    obey ALL but zZ debug code\n");
	console("-*z    obey ALL debug code except printnode\n");
	console("-*y... obey yflagged code (a..z)\n");
	console("-*x... obey xflagged code (A..Z)\n");
	console("-*f    debug output to:                    (ahcc.jnl)\n");
	console("-*l    all kinds of lists in journal       (%s)\n", ON(G.al_list_stats));
	console("-*d    suppress  optimizer                 (%s)\n", ON(G.ad_nopeep));
	console("-*n    suppress optimizer & assembler      (%s)\n", ON(G.an_no_O));
#endif
#if ! BIP_CC
	if (!MiNT and !MagX)
	{
		console(wacht);
		bios(2,2);
	}
#endif
}

static
void node_sizes(void)
{
	console("common        %ld\n", offsetof(struct vnode, nd));
	console("sizeof(XNODE) %ld\n", sizeof(XNODE));
	console("sizeof(TNODE) %ld\n", sizeof(TNODE));
	console("sizeof( NODE) %ld\n", sizeof( NODE));
	console("sizeof(INODE) %ld\n", sizeof(INODE));
	console("sizeof(OPND ) %ld\n", sizeof(OPND ));
	console("sizeof(BNODE) %ld\n", sizeof(BNODE));
	console("sizeof(SNODE) %ld\n", sizeof(SNODE));
	console("sizeof(FNODE) %ld\n", sizeof(FNODE));
	console("\n");
	console("sizeof(VNODE) %ld\n", sizeof(VNODE));
	console("sizeof(mnode) %ld\n", sizeof(struct mnode));
	console("sizeof(cnode) %ld\n", sizeof(struct cnode));
	console("sizeof(pnode) %ld\n", sizeof(struct pnode));
	console("sizeof(flown) %ld\n", sizeof(struct flown));
}

static
void setallflags(void)
{
	short i;
	G.anydebug++;
	console("'z-a = %d\n",dflags);
	for (i = 0; i < dflags; i++)
	{
		G.xflags[i] = 1;
		G.yflags[i] = 1;
	}
	debugZ = 0;
}

Cstr setflags(Cstr s, short *f)
{
	char c;

	while ((c = *s++) ne 0 )
	{
		c = tolower(c);
		if (c >= 'a' and c <='z')
		{
			f[c-'a'] ^= 1;		/* EOR usefull after options -*a  or -*z */
			G.anydebug++;
		}
	}

	return s;
}

static
void newopt(Cstr s)
{
	register char c;

	while ((c = *s++) ne 0 )
	{
		c = tolower(c);
		switch (c)
		{
		case 't':				/* was: no_registerization, now abolished (but see nopeep) */
		case 'r':				/*  "          "  */
			return;
#if DEBUG
		case 'x':
			s = setflags(s, G.xflags);
			return;
		case 'y':
			s = setflags(s, G.yflags);
			return;
#endif
#if C_DEBUG
		case 'a':		/* all except Z */
			setallflags();
			debugO = 0;	/* is a suppressor */
			continue;
		case 'z':		/* all except Z, N, O suppress printnode */
			setallflags();
			debugN = 0;
			debugO = 1;	/* is a suppressor */
			continue;
		case 'n':						/* No object output */
			ON_OFF(G.an_no_O);
		case 'd':						/* suppress optimizer */
			ON_OFF(G.ad_nopeep);
#endif
		case 'e':
			ON_OFF(G.ae_no_extracodes);		/* suppress extracodes (ahcc_rt.h) */
		case 'b':
			ON_OFF(G.ab_no_branch_reversals);
		case 'c':
			ON_OFF(G.ac_cache_headers);
		case 'f':
			ON_OFF(G.af_func_tree);
		case 'g':			/* no goto's */
			ON_OFF(G.ag_nogoto);
		case 'h':			/* project help */
			ON_OFF(G.ah_project_help);
		case 'j':
			ON_OFF(G.aj_auto_depend);
		case 'i':			/* default int is 32 bits */
			ON_OFF(G.ai_int32);
		case 'l':
#if NODESTATS
			G.al_list_stats = true;
			node_sizes();
#endif
			continue;
		case 'u':
			ON_OFF(G.au_supervisor);	/* default assembly .super */
		}
	}
}

short cdbv(Cstr s);
Cstr sis(Cstr s)
{
	if (*s eq '=') s++;
	return s;
}

/* 02'18 HR corrected wrong scope of me,mw,mi,
            then replaced by i = cdbv()
*/
/*		PURE_C compatible options		*/
static
void doopt(Cstr s)
{
	register char c;
	short i;
/*
	G.use_FPU = true;		/* 07'19 HR: v6 (no fp emulation) */
*/
	while ((c = *s++) ne 0 )
	{
		c = tolower(c);
		switch (c)
		{
		case 'a':
		case 'b':
		case 'g':			/* size optimization */
		case 'j':			/* Dont optimize jumps */
		case 'm':
		case 'n':
		case 'p':
		case 'q':
		case 's':
		case 't':
		case 'y':
		case 'z':
			return;;
		case 'c':				/* nested comments */
			ON_OFF(G.c_nested_comments);
		case 'd':
			adddef(sis(s));
			return;
		case 'e':				/* no of errors */
			i = cdbv(s);
			if (i)
				G.e_max_errors = i;
			return;
		case 'f':				/* no of warnings */
			i = cdbv(s);
			if (i)
				G.f_max_warnings = i;
			return;
		case 'h':			/* use cdecl calling */
			ON_OFF(G.h_cdecl_calling);
		case 'i':
			doincl(sis(s));
			return;
		case 'k':			/* default char is unsigned */
			ON_OFF(G.k_char_is_unsigned);
		case 'l':
			i = cdbv(s);
			if (i)
				G.l_identifier_max_length = i;;
			return;
		case 'u':
#if BIP_ASM
			if (G.xlang eq 's')	/* 10'12 HR: v4.15, ambiguous u option (undef)!!! */
				ON_OFF(G.au_supervisor);
			else
#endif
			{
				subdef(s);		/* 3'91 v1.2 */
				return;
			}
		case 'v':
#if ! BIP_CC
			if (G.v_Cverbosity eq 0)
				console(Version, ahcc_version);
#endif
			G.v_Cverbosity += 1;			/* number of v's = level of verbosity */
			continue;
		case 'w':
			return;
		case 'x':
			ON_OFF(G.x_add_underline);
#if COLDFIRE
		case '7':			/* Coldfire (double is 64 bits)  */
			ON_OFF(G.Coldfire);
#endif
#if COLDFIRE || FLOAT
		case '2':			/* generate for >= 68020 or CF compatible if also -7 */
			ON_OFF(G.i2_68020);
		case '3':
			ON_OFF(G.i2_68030);
		case '4':
			ON_OFF(G.i2_68040);
		case '6':
			ON_OFF(G.i2_68060);
#endif
#if FLOAT
		case '8':			/* generate for fpu MC68882 (reals) */
			ON_OFF(G.use_FPU);
#endif
#if ! BIP_CC
		default:
			help_options(c);
#endif
		}
	}
/*	console("use_FPU = %d\n", G.use_FPU);
*/
}

#include "cache.h"

global
short filecount, high_prj;

#define debug_l (G.yflags['l'-'a'])

#define prefix 128;

char in_name[256];

global
void cur_name(void)
{
	HI_NAME *hn = hn_make(G.inctab->name, ":\\.", 3);

	strcpy(in_name, hn->fn->n);
	hn_free(hn);
}

#if BIP_CC
global
void inc_stats(CP xp, bool i)
{
	stats.files++;
	if (i)
	{
		stats.I++;
		stats.cbytes += xp->bytes;
		stats.clines += xp->lines;
	othw
		stats.H++;
		stats.hbytes += xp->bytes;
		stats.hlines += xp->lines;
	}
}
#endif

global
VP load_bin(char *name)
{
	void *bitmap;
	short fl;
	long pl;
	VP fd = allocVn(1);

	if (fd)
	{
		bitmap = CC_load(name, &fl, &pl, "while loading", AH_CLOAD);
		if (bitmap)
		{
			char *s = CC_xmalloc(strlen(name) + 1, AH_OPEN_X, CC_ranout);
			strcpy(s, name);
			fd->name = s;
			fd->nflgs.f.nheap = 1;
			fd->codep = bitmap;
			fd->vval  = pl;
			type_is_not_a_copy(fd);			/* 10'12 HR 4.14 */
			return fd;
		}
		freeVn(fd);
		fd = nil;
	}

	return fd;
}

global
CP load_source_file(Cstr name, short *count)
{
	CP fd;
	short fl;
	long pl;
	char *bitmap;
	HI_NAME *hn;

	fd = new_cache();

	if (fd)
	{
		hn = hn_make(name, ":\\.", 3);
		strcpy(in_name, hn->fn->n);
		hn_free(hn);

		bitmap = Cload(name, &fl, &pl, "while loading");

		if (bitmap)
		{
			LEX_RECORD *lex;
			long cl, il = pl*(res_LEX) + prefix;		/* prefix allows for very small compact files */

			lex = CC_fmalloc(il, AH_OPEN_C, nil);
			if (lex)
			{
				long lines;
				char *s;

				cl = C_lexical(name, true, bitmap, lex, &lines, error, G.c_nested_comments,
#if BIP_ASM || FOR_A
				G.xlang
#else
				'c'
#endif
				);
/*				pr_lex(lex, "LEX:"); */
#if BIP_CC
				stats.bytes += pl;
				stats.lines += lines;
#endif
				lex = CC_frealloc(lex, cl, AH_OPEN_C, nil);
#if BIP_CC
				{
					DPP dp; short fileno = -1;
					Cstr iname;

					/* We must use the fileno if present of the prj db  */
					dp = pdb_find(prj_dependencies, name);		/* files from project */

					if (dp)
						fileno = dp->data->file_number;
					else
					{
						dp = pdb_find(auto_dependencies, name);
						if (!dp)
							fileno = ++filecount;
						else
							fileno = dp->data->file_number;
					}
					iname = G.inctab ? G.inctab->name : nil;

					if (count)
						*count = fileno;
					fd->fileno = fileno;

					if (G.aj_auto_depend or G.ah_project_help)
						pdb_fdepend(&auto_dependencies, iname, name, fileno);
				}
#else
				filecount++;
				if (count)
					*count = filecount;
				fd->fileno = filecount;
#endif
				fd->fl   = fl;

				G.C_bytes += pl;
				s = CC_xmalloc(strlen(name) + 1, AH_OPEN_X, CC_ranout);
				strcpy(s, name);
				fd->name = s;
				fd->heap  = 1;
				fd->text  = lex;
				fd->bytes = pl;
				fd->lines = lines;
				fd->size  = cl;
				CC_ffree(bitmap);
				return fd;
			othw
				CC_ffree(bitmap);
				cache = free_cache_unit(fd);		/* always last */
			}
		}
		else
			cache = free_cache_unit(fd);
	}

	return nil;
}

void pr_blocks(void);

static
VNODE start_inc;


global
short AHCC(short argc, char **argv)
{
#ifdef ENVINC
	Cstr pt;
#endif
	short shownames;

#ifdef CC_LEAK
/*	static long memory = 0, this = 0;
	if (memory eq 0)
		(void *)memory = Malloc(-1L);
	(void *)this = Malloc(-1L);		/* memory left !!! */
	if (this < memory)
	{
		XA_report punit;
		console("memory increased by %ld from %ld to %ld\n", memory - this, this, memory);
*/		XA_leaked(&XA_NS_base,  -1, -1, cpunit, 1);
		XA_leaked(&XA_CC_base,  -1, -1, cpunit, 2);
		XA_leaked(&XA_CC_fbase, -1, -1, cpunit, 3);
/*		memory = this;
	} */
#endif

	phase = COMPILING;
	init_cc();
	init_po();		/* 11'09 HR: fix serious regression */

#if BIP_CC
	{
		static bool SV = false;
		if (!SV)
		{
			console(Version, ahcc_version);		/* if integrated, only once */
/*			node_sizes();
*/			SV = true;
		}
	}
#endif

#ifdef ENVINC
	/*
	 * Parse the INCLUDE environment variable, if present.
	 */
	if ((pt = getenv("INCLUDE")) ne nil)
		doincl(pt);
#endif
	shownames = 0;

	while (argc-- > 1)
	{
#if BIP_ASM || FOR_A
		char * suf;
#endif
		argv++;			/* NB eerste arg = nil (gereserveerd voor program name */

		if (argv[0][0] eq '-')
		{
			if (argv[0][1] eq '*')
				newopt(&argv[0][2]);
			else
				doopt (&argv[0][1]);
		othw
#if ! BIP_CC
			if (!G.anydebug)
			{
				P_path av;
				S_path ern;

				av.s = argv[0];
	            ern = change_suffix(av.t, ".err");		/* errors file */
	            bugf = fopen(ern.s, "w");
	        }
#endif

#if BIP_ASM || FOR_A
			suf = getsuf(argv[0]);

			if (suf)			/* 09'16 HR v5.5 Prevent crashes on ill formed options.*/
			{
				strlwr(suf);
				G.xlang = *suf;
			}
			else
				G.xlang = 'c';
#endif
#if FOR_A
			if (suf and (*suf eq 'a' or *suf eq 'd'))
				G.xlang = 'a';

#endif
			input = load_source_file(argv[0], nil);

			{
				char *pd;

#if 0		/* simulate filename only */
				pd = strrslash(argv[0]);
				if (pd)
				{
					strcpy(argv[0], pd + 1);
				}
console("argv '%s'\n", argv[0]);
#endif
				DIRcpy(&G.input_dir, argv[0]);
				pd = strrslash(G.input_dir.s);
				if (pd)
				{
					*(pd + 1) = 0;
					DIRcpy(&G.includer, G.input_dir.s);	/* 07'11 HR: dont copy if no slash */
				}
				else
					G.input_dir.s[0] = 0;			/* 09'11 HR !!! */
			}

			if (input  eq nil)
			{
				console("Can't open input %s\n", argv[0]);
				succ = 1;
				continue;
			}

#if BIP_CC
			inc_stats(input, true);
#endif
			cache = cache->next;		/* only headers remain in cache */
			G.inctab = &start_inc;		/* for file_number */
			memset(G.inctab, 0, sizeof(VNODE));
			G.inctab          = allocVn(IFNODE);		/* level 0 for include chain */
			G.inctab->name    = argv[0];

#if BIP_CC
			G.inctab->p.fileno = input->fileno;
#endif

			if ( argc-- > 1 )
			{
				if (     argv[1][0] eq '-'
					and (   argv[1][1] eq 'o'
						 or argv[1][1] eq 'O'
						)
				   )
				{
					extern S_path o_name;  /* ex out.c */
					argv++;
					argc--;
					DIRcpy(&G.output_name, &argv[0][2]);
					o_name = G.output_name;
					if (!make_out())
					{
						succ = 1;
						continue;
					}
				}
			}
			if (argc > 0 or shownames) /* meer dan 1 input file */
			{
				shownames++;
				console              ("IN:  %s\t", G.inctab->name);
				if (G.output) console("OUT: %s", G.output_name.s);
				console("\n");
			}

			if (!out_setup(G.inctab->name))		/* fills output_name's */
			{
				succ = 1;
				continue;
			}


#if COLDFIRE
			if (G.Coldfire)
			{
				if (G.v_Cverbosity)
					console("target Coldfire\n");
				adddef("__DOUBLE_64__=1");
				adddef("__COLDFIRE__=1");
/*				G.i2_68020 = 0;
*/				G.use_FPU  = 1;
			}
#endif
#if FLOAT
			if (G.i2_68020)
			{
				if (G.v_Cverbosity)
					console("target 68020\n");
				adddef("__68020__=1");
	#if LL
				adddef("__LONGLONG__=1");
	#endif
			}
			if (G.use_FPU)
			{
				if (G.v_Cverbosity)
					console("target FPU\n");
				adddef("__68881__=1");
				adddef("__68882__=1");
				adddef("__FPU__=1");
			}
#else
			adddef("__NO_FLOAT__=1");
			adddef("__68000__=1");
			adddef("__LONGLONG__=0");
			if (G.v_Cverbosity)
				console("target 68000\n");
#endif
			if (G.k_char_is_unsigned)
				adddef("__CHAR_UNSIGNED__=1");

			if (G.ai_int32)
				adddef("__INT4__=1");
			else
				adddef("__MSHORT__=1");
#if FUNF
			adddef("__FUNF__=1");
#endif
#if C_DEBUG
			if (G.anydebug)
			{
				char pbug[56];
				Wstr pu = pbug;
				short i;
				for(i = 0; i<26; i++) if (G.xflags[i]) *pu++ = 'A'+i;
				for(i = 0; i<26; i++) if (G.yflags[i]) *pu++ = 'a'+i;
				*pu = 0;
				if (pbug[0])
					console("debug: %s\n", pbug);
			}
#endif
			if (argc > 0)
				succ |= do_file();	/*        not specific on no of  warnings */
			else
				succ  = do_file();	/* definitely     "       "         "	  */

			freedefs();				/* 10'12 HR: 4.14 */
#ifdef CC_LEAK
			XA_leaked(&XA_NS_base,  -1, -1, cpunit, 4);
			XA_leaked(&XA_CC_base,  -1, -1, cpunit, 5);
			XA_leaked(&XA_CC_fbase, -1, -1, cpunit, 6);
#endif
		}
	}

	free_srchlist();		/* -i directories */

	if (G.anydebug)
		console("\n");

	if (bugf ne stdout)
		fclose(bugf);	/* if BIP_CC bugf is ghost stdout */

#if ! BIP_CC || GEMSHELL
	if (!MiNT and !MagX)  /* Single TOS */  /* 06'16 HR v5.5 */
#endif
		waitexit(succ);

	return succ;
}

static
bool askq(void)
{
#if ! BIP_CC
	char c;
	if (G.anydebug)
	{
		breakin++;
		if (breakin > BRKVAL)
		{
			breakin = 0;
			console(corq);
			c = bios(2, 2);
			if (c eq 'q')
				return true;
		}
	}
#endif
	return false;
}

static
NP find_name(NP tp)
{
	if (tp)
		if (tp->nt ne DFNODE)
			if ( tp->tt ne E_LEAF)
				return find_name(tp->left);
	return tp;
}

static
char errbuf[256];

Cstr
	Error   = "Error",
	Warning = "Warning",
	Message = "Message",
	Comment = "Comment",
	W       = "Message+wait",
	Fatal   = "Fatal";

global
void errorn (void *vp, Cstr s, ...)
{
	NP tp = vp;
	long line = tp ? tp->fl.ln : line_no;
	va_list argpoint;

	if (toomanyerr())
		return;					/* last message allready given */
	optnl();
	va_start(argpoint, s);
	vsprintf(errbuf, s, argpoint);
	va_end(argpoint);
	if (G.inctab)
		console("%s   in %s L%ld %s", Error, G.inctab->name, line, errbuf);
	else
		console("%s   %s", Error, errbuf);
	if (tp)
	{
		console(" : '");
		if (tp->nt eq FLNODE)
			console("%s", tp->name);
		else
			send_name(tp->nt eq GENODE
							? childname(tp)
							: find_name(tp));
		console("'");
	}
	console("\n");
	G.nmerrors++;
#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif

#if WAIT
	Cconin();
#endif
}

global
void error(Cstr s, ...)
{
	va_list argpoint;
	if (toomanyerr())
		return;					/* last message allready given */
	optnl();
	va_start(argpoint, s);
	vsprintf(errbuf, s, argpoint);
	va_end(argpoint);

	if (G.inctab and G.inctab->name and *G.inctab->name)
		console("%s   in %s L%ld %s\n", Error, G.inctab->name, line_no, errbuf);
	elif (G.input_name.s and *G.input_name.s)
		console("%s   in %s L%ld %s\n", Error, G.input_name.s, line_no, errbuf);
	else
		console("%s   %s\n", Error, errbuf);

	G.nmerrors++;
#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif
#if WAIT
	Cconin();
#endif
}

global
void warnn(void *vp, Cstr s, ...)
{
	NP tp = vp;
	long line = tp ? tp->fl.ln : line_no;
	va_list argpoint;
	if (toomanywarn())
		return;					/* last message allready given */
	optnl();
	va_start(argpoint, s);
	vsprintf(errbuf, s, argpoint);
	va_end(argpoint);

	if (G.inctab and !G.eof)
		console("%s in %s L%ld %s", Warning, G.inctab->name, line, errbuf);
	else
		console("%s %s", Warning, errbuf);

	if (tp)
	{
		console(" : '");
		if (tp->nt eq FLNODE)
			console("%s", tp->name);
		else
			send_name(tp->nt eq GENODE
							? childname(tp)
							: find_name(tp));
		console("'");
	}

	console("\n");
	G.nmwarns++;
#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif
#if WAIT
	Cconin();
#endif
}

global
void warn(Cstr s, ...)
{
	va_list argpoint;
	if (toomanywarn())
		return;					/* last message allready given */
	optnl();
	va_start(argpoint, s);
	vsprintf(errbuf, s, argpoint);
	va_end(argpoint);
	if (G.inctab)
		console("%s in %s L%ld %s\n", Warning, G.inctab->name, line_no, errbuf);
	else
		console("%s %s\n", Warning, errbuf);
	G.nmwarns++;
#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif
#if WAIT
	Cconin();
#endif
}

global
Cstr src_name(void)
{
	if (*G.input_name.s)
		return G.input_name.s;
	return nil;
}

global
void messagen(void *vp, Cstr s, ...)
{
	NP tp = vp;
	Cstr n = (G.inctab) ? G.inctab->name : G.input_name.s;
	long line = tp ? tp->fl.ln : line_no;
	va_list argpoint;
	optnl();
	va_start(argpoint, s);
	vsprintf(errbuf,s, argpoint);
	va_end(argpoint);

	if (n)
		console("%s in %s L%ld %s", Comment, n, line, errbuf);
	else
		console("%s %s", Comment, errbuf);

	if (tp)
	{
		console(" : '");
		send_name(tp->nt eq GENODE
						? childname(tp)
						: find_name(tp));
		console("'");
	}

	console("\n");
#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif
}

global
void message(Cstr s, ...)
{
	Cstr n = (G.inctab) ? G.inctab->name : G.input_name.s;
	va_list argpoint;
	va_start(argpoint, s);
	vsprintf(errbuf, s, argpoint);
	va_end(argpoint);

	if (n)
		console("%s in %s L%ld %s\n", Comment, n, line_no, errbuf);
	else
		console("%s %s\n", Comment, errbuf);

#if ! BIP_CC
	if (bugf eq stdout) if ( askq() ) exit(1);
#endif
}

global
void waitexit(short ret)		/* also used in nodes.c */
{
#if ! BIP_CC
	if (   G.nmerrors
		or G.nmwarns)
	{
		console(wacht);
		bios(2, 2);
	}
	exit(ret);
#endif
}
