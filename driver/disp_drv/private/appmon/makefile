TARGET = appmonitor
TARGETDIR = ..\..\bin
DDK_BASE = \WINDDK\3790.1830
INCLUDE=$(DDK_BASE)\INC\CRT;$(DDK_BASE)\INC\DDK;$(INCLUDE)

ASM = ml
CPP = cl
RSC = rc.exe
F90 = df.exe
MTL = midl.exe
REBASE = rebase.exe

OBJDIR = .\obj\i386

ASM_PROJ=/coff /c /Fo$(OBJDIR)\\

RES_PROJ=/I "..\..\..\..\inc" \
        /D "WIN32" /D "_WINDOWS" /fo$(OBJDIR)\appmon.res


CPP_PROJ=/nologo /MD /W3 /Oxs /Gz /Zi  \
	/I "$(DDK_BASE)\inc" \
	/I "$(DDK_BASE)\inc\wnet" \
        /I "$(DDK_BASE)\inc\ddk" \
        /I "$(DDK_BASE)\src\video\inc" \
        /D "WIN32" /D "_WINDOWS" \
	/Fr$(OBJDIR)\\ /Fo$(OBJDIR)\\ /Fd$(OBJDIR)\\ /c

LIB32= link.exe 
LIB32_FLAGS = /LIBPATH:"C:\Program Files\Microsoft Visual Studio\VC98\Lib" /LIBPATH:$(DDK_BASE)\lib\wxp\i386  /DEBUG /PDB:$(TARGETDIR)\SYMBOLS\$(TARGET).PDB  /SUBSYSTEM:WINDOWS /nologo $(LIBS) /out:$(TARGETDIR)\$(TARGET).exe

RESFILE = \
          $(OBJDIR)\appmon.res

OBJS   = \
	$(OBJDIR)\appmon.obj \
        $(OBJDIR)\virtualmonitor.obj \
        $(OBJDIR)\entry.obj \
        $(OBJDIR)\videomemory.obj \
        $(OBJDIR)\gdi.obj


LIBS = \
       kernel32.lib \
       Advapi32.lib \
       user32.lib \
       gdi32.lib

 
# This is a comment
$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS) $(RESFILE)
    $(LIB32) $(LIB32_FLAGS) $(OBJS) $(LIBS) $(RESFILE)
    $(REBASE) -b 0x00400000 -x $(TARGETDIR)\SYMBOLS -a $(TARGETDIR)\$(TARGET).exe

{.\}.c{$(OBJDIR)}.obj::
	$(CPP) $(CPP_PROJ) $<

{.\}.cpp{$(OBJDIR)}.obj::
	$(CPP) $(CPP_PROJ) $<


{.\}.asm{$(OBJDIR)}.obj::
	$(ASM) $(ASM_PROJ) $<


{.\}.rc{$(OBJDIR)}.res::
	$(RSC) $(RES_PROJ) $<

$(OBJDIR):
    if not exist "$(OBJDIR)/$(NULL)" mkdir "$(OBJDIR)"

$(TARGETDIR):
    if not exist "$(TARGETDIR)/$(NULL)" mkdir "$(TARGETDIR)"


CLEAN:
	-@erase /S /Q $(OBJDIR)
