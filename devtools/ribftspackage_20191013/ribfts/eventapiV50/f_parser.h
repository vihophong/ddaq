#include "typedefs.h"
#ifndef __PARSER__
#define __PARSER__ Version vom 03.09.1993

#include <stdio.h>
#include "parsdef.h" 		/* Definitionsfile			*/

struct Key
{	CHARS id[PARS__DEF_LEN];	/* Name des Key's			*/
	CHARS type[4];		/* Datentyp und optional/required	*/
	INTS4 anz;		/* Anzahl der Elemente			*/
	INTS4 nr;			/* Key-Nummer				*/
	REAL4 von,bis;		/* Wertebereich				*/
				/* Wenn Von >0 und bis =0 sind das die	*/
				/* Anzahl der Stellen			*/
	struct Key *prev, *next;/* Verkettung				*/
};

struct Tabelle			/* hier werde die eigentlichen Werte	*/
{	struct Tabelle *next;	/* abgelegt. next = Verkettung		*/
	CHARS *werte;		/* Zeiger auf die Werte			*/
};

struct Facility
{	CHARS id[PARS__DEF_LEN];	/* Name der facility			*/
	INTS4 anz;		/* Anzahl Keys / Facility		*/
	INTS4 size;		/* Groesse des kompletten facilitzs in	*/
				/* Bytes				*/
	struct Key *key;	/* Verkettung zu den Schluesselwoertern	*/
	struct Tabelle *wert;	/* Verkettung mit Tabelle		*/
	struct Tabelle **akt;	/* Aktuelle Position in Tabelle		*/
	struct Facility *next;	/* Verkettung zu der naechsten facility	*/
};

typedef struct Facility *FAC;
typedef struct Tabelle *TAB;

/***************** ANSI-Funktionsdeklarationen **************************/
INTS4 	f_pars_set(FAC f_fac_start, CHARS *filename);
FAC 	f_pars_def(CHARS *c_def_file);
FAC 	f_pars_sfac(CHARS *pc_fac,FAC ps_fac,TAB *table);
struct Key *f_pars_skey(CHARS *pc_id,FAC ps_fac, INTS4 *offset);
CHARS 	*f_get_val(CHARS *pc_keyid,FAC ps_fac,TAB *ps_tab,INTS4 *anz,CHARS *typ,
			INTS4 next);
INTS4 	*f_pars_l_get(CHARS *pc_keyid,INTS4 *anz,TAB *ps_tab,FAC ps_fac, INTS4 next);
INTU4 *f_pars_ul_get(CHARS *pc_keyid,INTS4 *anz,TAB *ps_tab,FAC ps_fac,
		 INTS4 next);
REAL4 	*f_pars_f_get(CHARS *pc_keyid,INTS4 *anz,TAB *ps_tab,FAC ps_fac, INTS4 next);
CHARS 	*f_pars_c_get(CHARS *pc_keyid,INTS4 *anz,TAB *ps_tab,FAC ps_fac, INTS4 next);
CHARS 	*f_pars_tab_get(TAB *ps_tab,FAC ps_fac);
void 	f_pars_fac_free(FAC start);
void 	f_pars_fac_out(FAC ps_list);
#endif
