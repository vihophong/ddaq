# Hacked from Microsoft Developer Studio Generated NMAKE File

CPP=cl.exe

OUTDIR=.
OBJDIR=.
SRCDIR=..

ALL : "$(OUTDIR)\peek.exe"

CLEAN :
        -@erase "$(OBJDIR)\*.obj"
        -@erase "$(OBJDIR)\*.pch"
        -@erase "$(OBJDIR)\vc60.idb"
        -@erase "$(OUTDIR)\peek.exe"

INSTALL:
      cp $(OUTDIR)\peek.exe  ..\..\..\MIDAS-WIN32\bin\

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "./" /I "../" /I "../../" /I "../../../"  \
/D "LINK"  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" \
/Fp"$(OBJDIR)\link.pch" /YX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" /FD /c

LINK32=link.exe
LINK32_FLAGS= wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
 /nologo /subsystem:console /incremental:no \
 /pdb:"$(OUTDIR)\link.pdb" /machine:I386 /out:"$(OUTDIR)\peek.exe"
LINK32_OBJS= \
        "$(OBJDIR)\peek.obj"


"$(OUTDIR)\peek.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

{$(SRCDIR)}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

{..\$(SRCDIR)}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<
