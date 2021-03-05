/* Message codes of facility GOOIO */
/* This file is produced on the VAX by command */
/* $ MESDEF GOOVME                        */
#define XIO_SUCCESS                          0x08678009
/*  Success condition: !AS */
#define XIO_INFORM                           0x08678013
/*  Informational condition: !AS */
#define XIO_WARNING                          0x08678018
/*  Warning condition: !AS */
#define XIO_ERROR                            0x08678022
/*  Error condition: !AS */
#define XIO_FATAL                            0x0867802C
/*  Fatal condition: !AS */
#define XIO_ILLBDO                           0x08678032
/*  Illegal record format for MBD object module !AS */
#define XIO_BADBDO                           0x0867803A
/*  Inconsistent MBD channel program !AS */
#define XIO_BADJCDRV                         0x08678042
/*  !AS is not a valid reference to the MBD driver */
#define XIO_BADMBDVFY                        0x0867804A
/*  !AS mismatches during MBD load verification */
#define XIO_BADMBDEXEC                       0x08678052
/*  Invalid MBD executive found in "!AS", base is !AS */
#define XIO_NOTMBDEXEC                       0x0867805A
/*  Invalid MBD executive found in "!AS", channel is !AS */
#define XIO_ISMBDEXEC                        0x08678062
/*  The file !AS may contain a MBD executive */
#define XIO_BADCAMACB                        0x0867806A
/*  Bad CAMAC branch number "!AS" specified */
#define XIO_BADCAMACC                        0x08678072
/*  Bad CAMAC crate number "!AS" specified */
#define XIO_BADCAMACN                        0x0867807A
/*  Bad CAMAC station number "!AS" specified */
#define XIO_BADCAMACA                        0x08678082
/*  Bad CAMAC subaddress "!AS" specified */
#define XIO_BADCAMACF                        0x0867808A
/*  Bad CAMAC function "!AS" specified */
#define XIO_BADCAMACR                        0x08678092
/*  Bad CAMAC repeat count "!AS" specified */
#define XIO_BADCAMACD                        0x0867809A
/*  Illegal dimension "!AS" specified in CAMAC call */
#define XIO_BADCAMACE                        0x086780A2
/*  Illegal external address specified in CAMAC call */
#define XIO_BADCAMACL                        0x086780AA
/*  Illegal LAM identifier specified in CAMAC call */
#define XIO_CAMXQ                            0x086780B1
/*  CAMAC action completed with X and Q */
#define XIO_CAMXNOQ                          0x086780B9
/*  CAMAC action completed with X but no Q */
#define XIO_CAMNOXQ                          0x086780C1
/*  CAMAC action completed with no X but with Q (STRANGE) */
#define XIO_CAMNOXNOQ                        0x086780C9
/*  CAMAC action completed with no X and no Q */
#define XIO_TSKTOBIG                         0x086780D2
/*  TASK image too big, base=!AS, size=!AS */
#define XIO_BADJ11VFY                        0x086780DA
/*  Load verification failed for STARBURST in C=!AS */
#define XIO_AQUSTARTED                       0x086780E0
/*  Acquisition already started */
#define XIO_AQUSTOPPING                      0x086780E8
/*  Acquisition has been stopped, but the stop did not complete. */
#define XIO_AQUNOTSTARTED                    0x086780F0
/*  Acquisition not yet started */
#define XIO_CTSSTOPPED                       0x086780FB
/*  Test !AS on C=!AS N=!AS already stopped */
#define XIO_CTSACTIVE                        0x08678102
/*  Test !AS on C=!AS N=!AS already active */
#define XIO_CTSBADDELTA                      0x0867810A
/*  Bad delta time !AS for test reschedule */
#define XIO_BADINDEX                         0x08678112
/*  An illegal index has been specified */
#define XIO_AQUNOINIT                        0x0867811A
/*  Acquisition not yet initialized */
#define XIO_AQUINITDONE                      0x08678122
/*  Acquisition already initialized */
#define XIO_LMDSTARTED                       0x0867812A
/*  List mode dump already started */
#define XIO_LMDSTOPPING                      0x08678132
/*  List mode dump already stopped */
#define XIO_LMDNOTSTARTED                    0x0867813A
/*  List mode dump not yet started */
#define XIO_LMDNOTOPENED                     0x08678142
/*  List mode file not opened */
#define XIO_BADDEFREC                        0x0867814A
/*  Syntax error in definition file record !AS */
#define XIO_BADDEFVFY                        0x08678152
/*  Verification error in !AS for !AS */
#define XIO_CTSERR                           0x0867815A
/*  Error in CAMAC test of !AS at !AS!/ Syndrom:!AS */
#define XIO_CTSERRSTOP                       0x08678162
/*  Error limit of !AS exceeded for !AS at !AS */
#define XIO_ILLNULLPTR                       0x0867816A
/*  Illegal NULL pointer passed as argument */
#define XIO_AQUBADBUF                        0x08678172
/*  Bad buffer structure, length !AS at offset !AS */
#define XIO_ILLMODE                          0x0867817A
/*  An illegal mode !AS has been specified */
#define XIO_BUGCHK                           0x08678182
/*  BUGCHECK. Internal consistency check, syndrome: !AS */
#define XIO_BADCAMACCI                       0x0867818A
/*  Bad CAMAC channel identifier "!AS" specified */
#define XIO_LMDOPENED                        0x08678192
/*  List mode file already opened */
#define XIO_J11TABERR                        0x0867819A
/*  J11 configuration table error in: !/!_!_!AS!/!_!AS!/!_Item: "!AS"  Value: "!AS" */
#define XIO_UNKNBUF                          0x086781A0
/*  Unknown buffer type !AS, subtype !AS */
#define XIO_UNKNEVT                          0x086781A8
/*  Unknown event type !AS, subtype !AS at !/!_!_buffer data index = !AS. */
#define XIO_NOMOREEVENT                      0x086781B0
/*  No more events in buffer */
#define XIO_BADEVENT                         0x086781B8
/*  Bad event found: type=!AS, subtype=!AS,!/!_!_buffer index = !AS. */
#define XIO_BUFFERFULL                       0x086781C0
/*  List mode buffer full */
#define XIO_EVTOOBIG                         0x086781C8
/*  Event is too big for buffer */
#define XIO_OUTSTART                         0x086781D0
/*  Analysis output already started */
#define XIO_OUTNOSTART                       0x086781D8
/*  Analysis output not started */
#define XIO_SKIPEVENT                        0x086781E0
/*  Skip event */
#define XIO_FILENOTOPEN                      0x086781E8
/*  File not open */
#define XIO_FILESTILLOPEN                    0x086781F0
/*  File still open */
#define XIO_LOADERROR                        0x086781FA
/*  Error loading module !AS in sharable image !AS */
#define XIO_NOOUTPUT                         0x08678200
/*  No event for output */
#define XIO_NOEVENTDE                        0x0867820A
/*  There is no Data Element to copy input event */
#define XIO_EVTLOOPER                        0x08678210
/*  An error occured in buffer !AS,!/!_!_event !AS, buffer event !AS. */
#define XIO_STOPINPUT                        0x08678218
/*  Stop input */
#define XIO_STOPOUTPUT                       0x08678220
/*  Stop output */
#define XIO_INMBXSTOP                        0x08678228
/*  Input from mailbox already stopped */
#define XIO_INNETSTOP                        0x08678230
/*  Input from DECnet already stopped */
#define XIO_INFILSTOP                        0x08678238
/*  Input from file already stopped */
#define XIO_BADMBDEVENT                      0x08678240
/*  Bad MBD event found: !/!_!_!AS. */
#define XIO_EVDETOOSMALL                     0x08678248
/*  Event data element is too small for event of size !AS. */
#define XIO_VMETABERR                        0x08678252
/*  VME configuration table error in: !/!_!_!AS!/!_!AS!/!_Item: "!AS"  Value: "!AS" */
#define XIO_MOUNTERR                         0x0867825A
/*  Tape mount error: !/!_!_!AS */
#define XIO_VMEDESTERR                       0x08678262
/*  VME destination processor error: !/!_!_!AS */
#define XIO_ES_LSTBUF                        0x08678269
/*  Last esone buffer */
#define XIO_ES_MAXCLN                        0x08678272
/*  Esone server already serves too many clients */
#define XIO_ES_EESONE                        0x0867827A
/*  Esone server error */
#define XIO_ES_CNAFERR                       0x08678282
/*  Invalid CNAF */
