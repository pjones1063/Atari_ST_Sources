/*
 * ListDataBase_Public.h
 */
void	ListDB_ENPG(
			VOIE	*	pVoie_curr,		/* In: Voie concern�e */
			int		n_DBFnct );		/* In: Fonction Db � utiliser */
void	ListDB_OPDB(
			VOIE	*	pVoie_curr );		/* In: Voie concern�e */
void	ListDB_DispData(
			VOIE *	pVoie_curr );		/* In: Voie concern�e */
void	ListDB_DispPage(
			VOIE *	pVoie_curr );		/* In: Voie concern�e */

BOOL	ListDB_CmdDelRec(					/* Out: !FALSE si succ�s */
			VOIE	*	pVoie_curr,			/* In:  Voie concern�e */
			char	*	piBsz_Arguments );	/* In:  Arguments pass�s derri�re la commande */

void	ListMsg_KeyAction( 
			KEYCODE	curr_keypress, /* In: Touche press�e */
			VOIE	*	pVoie_curr );	/* In: Voie concern�e */
