# Hacked from Microsoft Developer Studio Generated NMAKE File

ITEM=main

CPP=cl.exe

OUTDIR=.
OBJDIR=.

ALL : "$(OUTDIR)\$(ITEM).exe"

CLEAN :
        -@erase "$(OBJDIR)\*.obj"
        -@erase "$(OUTDIR)\$(ITEM).exe"
        -@erase "$(OUTDIR)\$(ITEM).pch"
        -@erase "$(OBJDIR)\vc*.idb"


"$(OUTDIR)" :
    if not exist "$(OUTDIR)/" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo  /W0 /EHsc /O2 /I "./" /I "../" /I "../../xfer/include" /I "../../VHSADC_RTDEX" /D "TSOVERLAP" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" \
    /D "_MBCS"  /Fp"$(OBJDIR)\$(ITEM).pch" /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" /FD /c /D "_CRT_SECURE_NO_WARNINGS"

LINK32=link.exe
LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib \
     ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     wsock32.lib pthreadVC2.lib \
    /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\$(ITEM).pdb" /machine:I386 \
    /out:"$(OUTDIR)\$(ITEM).exe" 

LINK32_OBJS=   "$(OBJDIR)\$(ITEM).obj" "$(OBJDIR)\transfer.obj"


"$(OUTDIR)\$(ITEM).exe" : "$(OUTDIR)"  $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS)  $(LINK32_OBJS)
<<

{..\}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

{.\}.c{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

{..\}.cpp{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

{.\}.cpp{$(OBJDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<


