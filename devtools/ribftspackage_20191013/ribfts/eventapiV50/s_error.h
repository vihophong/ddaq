#include "typedefs.h"
/* ++++++++++++++++++++++
 * +++   s_error.h    +++
 * ++++++++++++++++++++++
 * RSMayer
 * 07-Feb-1994
 * structure with error numbers and error text
 *
 * Modification:
 *   23-Mar-1994 RSMayer EVent_server error numbers
 *   25-Mar-1994 RSMayer ERR__SIGNAL, SETTASK, SETVERB, CLRVERB added.
 *   31-Mar-1994 RSMayer ERR__USBFTY, ERR__NKNBFTY.
 *   12-Jul-1994 RSMayer Error numbers and msg. for Esone Server
 *   16-Feb-1995 RSMayer new error numbers.
 *   08-Mar-1995 RSMayer some basic error number text modified.
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* !!!! error numbers MUST be changed in errnum_def.h TOO !!!! */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* errnum_def.h  must be included BEFORE! */

/* && is an optional placeholder          */

struct s_err 
       {
          INTS4 l_errno;
          CHARS *c_const;
          CHARS *c_errtxt;
       };

/* +++ UTIlity      error numbers start with   0 +++ */
/* +++ COLlector    error numbers start with 100 +++ */
/* +++ RD (Readout) error numbers start with 200 +++ */
/* +++ TRansport    error numbers start with 300 +++ */
/* +++ EVent_server error numbers start with 400 +++ */

static struct s_err s_error[] =
       {
          { ERR__SUCCESS,  "SUCCESS",
          "Normal successfull completion &&"},
          { ERR__ERROR,  "ERROR",
          "Error: &&."},
          { ERR__INFO,  "INFO",
          "&&."},
          { ERR__ERR  ,  "ERR",
          "&&."},
          { ERR__WARN ,  "WARN",
          "Warning: &&."},
          { ERR__ERRNF,  "ERRNF",
          "Error number && not found"},
          { ERR__MSKNF,  "MSKNF",
          "Mask && not found"},
          { ERR__MSKNIMPL,  "MSKNIMPL",
          "Mask && not implemented"},
          { ERR__FNIMPL,  "FNIMPL",
          "Feature not implemented. &&."},
          { ERR__SYSERR,  "SYSERR",
          "System error: &&."},
          { ERR__SIGNAL,  "SIGNAL",
          "Signal: &&."},
          { ERR__SETTASK,  "SETTASK",
           "Set task name: &&."},
          { ERR__SETVERB,  "SETVERB",
           "Verbosity flag set: &&."},
          { ERR__CLRVERB,  "CLRVERB",
           "Verbosity flag cleared: &&."},
          { ERR__STS,  "STS",
/*           "f_ut_error-status: &&."},
 */
           "errstat &&."},
          { ERR__INIT,  "INIT",
           "Using f_ut_error local ctrl structure."},
          { ERR__INITERR,  "INITERR",
           "Warn: Ctrl structure not specified, using local one."},
          { ERR__INITGET,  "INITGET",
           "Get f_ut_error ctrl structure."},
          { ERR__INITADDR,  "INITADDR",
           "Invalid addr: Read or write f_ut_error ctrl structure."},
          { ERR__INITPUT,  "INITPUT",
           "Put f_ut_error ctrl structure. &&."},
          { ERR__TEST,  "TEST",
           "Test f_ut_error: &&."},
          { ERR__ENALOG,  "ENALOG",
           "Enable message log: &&."},
          { ERR__DISLOG,  "DISLOG",
           "Disable message log: &&."},

          { ERR__COL_HISENA,  "COL_HISENA",
          "Histogramming already enabled. &&"},

          { ERR__EV_CLNOTMR,  "EV_CLNOTMR",
          "Warning: Client not marked for deletion. &&"},
          { ERR__EV_CLAMRK,  "EV_CLAMRK",
          "Warning: Client already marked for deletion. &&"},
          { ERR__EV_ECLAMRK,  "EV_ECLAMRK",
          "Client already marked for deletion. &&"},
          { ERR__EV_CLNOTV,  "EV_CLNOTV",
          "Warning: Client not valid. &&"},
          { ERR__EV_ECLNOTV,  "EV_ECLNOTV",
          "Client not valid. &&"},
          { ERR__EV_CLNRDY,  "EV_CLNRDY",
          "No client ready/active. &&"},
          { ERR__EV_FLTNV,  "EV_FLTNV",
          "Filter not valid. &&"},
          { ERR__EV_WARNFLT,  "EV_WARNFLT",
          "Warning for filter. &&"},
          { ERR__EV_EBFACKN,  "EV_EBFACKN",
          "Buffer acknoledge. &&"},
          { ERR__EV_WBFACKN,  "EV_WBFACKN",
          "Warning: Buffer acknoledge. &&"},
          { ERR__EV_WBACKTO,  "EV_WBACKTO",
          "Warning: Buffer acknoledge timeout. &&"},
          { ERR__EV_WBFPEND,  "EV_WBFPEND",
          "Buffer pending. &&"},
          { ERR__EV_CLNSTS,  "EV_CLNSTS",
          "Wrong client status. &&"},
          { ERR__EV_LSTBUF,  "EV_LSTBUF",
          "Info: Was last buffer. &&"},
          { ERR__EV_WRTBUF,  "EV_WRTBUF",
          "Error writing buffer. &&"},
          { ERR__EV_CLNACC,  "EV_CLNACC",
          "Error client accept. &&"},
          { ERR__EV_RELSTR,  "EV_RELSTR",
          "Release stream: &&."},
          { ERR__EV_USBFTY,  "EV_USBFTY",
          "Unsupported buffer typ: &&."},
          { ERR__EV_UKNBFTY,  "EV_UKNBFTY",
          "Unknown buffer typ: &&."},
          { ERR__EV_UKNEVTY,  "EV_UKNEVTY",
          "Unknown event typ: &&."},
          { ERR__EV_UKNSETY,  "EV_UKNSETY",
          "Unknown subevent typ: &&."},
          { ERR__EV_EVTOOBIG,  "EV_EVTOOBIG",
          "Event too big for buffer: &&."},

          { ERR__ES_CLAMRK,  "ES_CLAMRK",
          "Warning: Client already marked for deletion. &&"},
          { ERR__ES_CLNOTMR,  "ES_CLNOTMR",
          "Warning: Client not marked for deletion. &&"},
          { ERR__ES_BFRDTO,  "ES_BFRDTO",
          "Buffer read timeout. &&"},
          { ERR__ES_ERDBUF,  "ES_ERDBUF",
          "Error reading buffer. &&"},
          { ERR__ES_LSTBUF,  "ES_LSTBUF",
          "Info: Was last buffer. &&"},
          { ERR__ES_WRTBUF,  "ES_WRTBUF",
          "Error writing buffer. &&"},
          { ERR__ES_CLNACC,  "ES_CLNACC",
          "Error client accept. &&"},
          { ERR__ES_MAXCLN,  "ES_MAXCLN",
          "Maximum number of clients exceeded. &&"},
          { ERR__ES_BFSTRD,  "ES_BFSTRD",
          "Buffer status read: &&"},
          { ERR__ES_BFSTWR,  "ES_BFSTWR",
          "Buffer status write: &&"},
          { ERR__ES_BFPROC,  "ES_BFPROC",
          "Buffer in process: &&"},
          { ERR__ES_EESONE,  "ES_EESONE",
          "Error in Esone call: &&"},
/* */        
          { ERR__HIS_SPNFND,  "HIS_SPNFND",
          "Error: Histogram && not found."},
          { ERR__HIS_EOFSPN,  "HIS_EOFSPN",
          "Warn: End of histogram list. && "},
          { ERR__HIS_RANGES,  "HIS_RANGES",
          "Error: Value out of range. &&"},
          { ERR__HIS_WRNGNO,  "HIS_WRNGNO",
          "Error: Wrong number typed &&"},
          { ERR__HIS_EANAL ,  "HIS_EANAL",
          "Error in analysis: &&"},
          { ERR__HIS_NFREEM,  "HIS_NFREEM",
          "Error: Can't get memory. &&"},
          { ERR__HIS_NOPENF,  "HIS_NOPENF",
          "Error: Can't open the file &&"},
          { ERR__HIS_INVHIS,  "HIS_INVHIS",
          "Error: Invalid histogram name or too long. &&"},
          { ERR__HIS_NALFNO,  "HIS_NALFNO",
          "Error: Only alphanumeric symbols allowed. &&"},
          { ERR__HIS_NOMEMR,  "HIS_NOMEMR",
          "Error: No memory allocated. &&"},
          { ERR__HIS_HALREX,  "HIS_HALREX",
          "Error: Histogram && already exists."},
          { ERR__HIS_NFREDP,  "HIS_NFREDP",
          "No free memory in data pool &&"},
          { ERR__HIS_HTABFU,  "HIS_HTABFU",
          "Histogram table full &&"},
          { ERR__HIS_EWPAR ,  "HIS_EWPAR",
          "Wrong parameter &&"},
          { ERR__HIS_SKIPEVT,  "HIS_SKIPEVT",
           "Skip event &&"},
          { ERR__HIS_SKIPSEV,  "HIS_SKIPSEV",
           "Skip subevent &&"},
          { ERR__HIS_NBASATT,  "HIS_NBASATT",
           "No Base attached. &&"},
          { ERR__HIS_BASATT,  "HIS_BASATT",
           "Base && already attached."},
          { ERR__HIS_NHISBAS,  "HIS_NHISBAS",
           "Base && is not a histogram base."},
          { ERR__HIS_DBLOCK,  "HIS_DBLOCK",
           "Base locked by another user. &&"},
/*          { ERR__,  "",
 *         "."},
 */
          {9999, "ENDLIST",
          "das ist KEIN Template!! nimm die anderen!"}
       };
