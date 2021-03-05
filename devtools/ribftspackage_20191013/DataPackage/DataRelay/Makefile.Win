# Hacked from Microsoft Developer Studio Generated NMAKE File

CPP=cl.exe

OUTDIR=..\..\TS_bin\Win
OBJDIR=.

WIN32BIN=..\..\..\MIDAS-WIN32\bin

ALL : "$(OUTDIR)\TSrelayTCP.exe"

CLEAN :
        -@erase "$(OBJDIR)\*.obj"
        -@erase "$(OBJDIR)\*.pch"
        -@erase "$(OBJDIR)\vc*.idb"
        -@erase "$(OUTDIR)\TSrelayTCP.exe"

INSTALL:
      cp $(OUTDIR)\TSrelayTCP.exe  ..\..\..\MIDAS-WIN32\bin\

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "./" /I "../" /I "../../" /I "../../../"  \
/D "SINK"  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" \
/Fp"$(OBJDIR)\link.pch" /YX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" /FD /c

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
  wsock32.lib tslib.lib   /libpath:"$(WIN32BIN)" \
 /nologo /subsystem:console /incremental:no \
 /pdb:"$(OUTDIR)\link.pdb" /machine:I386 /out:"$(OUTDIR)\TSrelayTCP.exe"
LINK32_OBJS= \
        "$(OBJDIR)\main.obj" \
        "$(OBJDIR)\midasTS.obj"

"$(OUTDIR)\TSrelayTCP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

{..\..\relayTCP}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

{..}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<


{..\..\}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

