#include "typedefs.h"
/*######################################################################*/
/************************************************************************/
/**** Definitionsfile fuer Paserfunktionen ******************************/
/************************************************************************/
/*######################################################################*/
/* Modified: 29-Oct-1993 value changed                     RSM          */
/* 
/* -------------------------------------------------------------------- */

/************************************************************************/
/****************** Definitionen fuer 'feste' groessen ******************/
/************************************************************************/
#define PARS__STR_LEN 4095		/* Zeilenlaenge gesammt	Zeichen	*/
#define PARS__FNM_LEN 255			/* Maximale laenge eines Filenamens */
#define PARS__DEF_LEN 32			/* Länge eines Defines in Parser*/
                                        /*   was 30 before!!!           */
					/* Definitionsfile		*/

/************************************************************************/
/**** Definitionen fuer 'Syntaxerkennung' im Paser Definitions File *****/
/************************************************************************/
#define PARS__Z_TRENN '-'			/* Trennzeichen zwichen Zeilen	*/
					/* die zusammengehoeren		*/
#define PARS__KOMMENTAR_Z '#'		/* Zeichen, das Kommentare in	*/
					/* den zu lesenden Files einleitet */
#define PARS__DEF_CODE "DEF"		/* Womit werden Definitionen	*/
					/*  eingeleitet			*/
#define PARS__FILE_INS_CODE '@'		/* Womit wird ein einzufuegendes */
					/* File definiert		*/
#define PARS__DEF_INSERT '$'		/* Einfuegen			*/
#define PARS__FACILITY_DEF ':'		/* Davor ist Facilityname	*/
