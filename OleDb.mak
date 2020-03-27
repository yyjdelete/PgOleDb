# Microsoft Developer Studio Generated NMAKE File, Based on OleDb.dsp
!IF "$(CFG)" == ""
CFG=OleDb - Win32 Debug
!MESSAGE No configuration specified. Defaulting to OleDb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OleDb - Win32 Debug" && "$(CFG)" != "OleDb - Win32 Unicode Debug" && "$(CFG)" != "OleDb - Win32 Release MinSize" && "$(CFG)" != "OleDb - Win32 Release MinDependency" && "$(CFG)" != "OleDb - Win32 Unicode Release MinSize" && "$(CFG)" != "OleDb - Win32 Unicode Release MinDependency"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OleDb.mak" CFG="OleDb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OleDb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OleDb - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OleDb - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OleDb - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OleDb - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OleDb - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OleDb - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" "$(OUTDIR)\OleDb.bsc" ".\Debug\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\OleDb.sbr"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgCommand.sbr"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgDs.sbr"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgMultipleResults.sbr"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgRS.sbr"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\PgSess.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\TypeInfo.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\OleDb.bsc"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.ilk"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase "$(OUTDIR)\PgOleDb.pdb"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\OleDb.sbr" \
	"$(INTDIR)\PgCommand.sbr" \
	"$(INTDIR)\PgDs.sbr" \
	"$(INTDIR)\PgRS.sbr" \
	"$(INTDIR)\PgSess.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TypeInfo.sbr" \
	"$(INTDIR)\PgMultipleResults.sbr"

"$(OUTDIR)\OleDb.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PgOleDb.pdb" /debug /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /pdbtype:sept /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\PgOleDb.dll
InputPath=.\Debug\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" ".\OleDb.tlb" ".\OleDb.h" ".\OleDb_i.c" "$(OUTDIR)\OleDb.bsc" ".\DebugU\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\OleDb.sbr"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgCommand.sbr"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgDs.sbr"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgMultipleResults.sbr"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgRS.sbr"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\PgSess.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\TypeInfo.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\OleDb.bsc"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.ilk"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase "$(OUTDIR)\PgOleDb.pdb"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\DebugU\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /ZI /Od /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\OleDb.sbr" \
	"$(INTDIR)\PgCommand.sbr" \
	"$(INTDIR)\PgDs.sbr" \
	"$(INTDIR)\PgRS.sbr" \
	"$(INTDIR)\PgSess.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TypeInfo.sbr" \
	"$(INTDIR)\PgMultipleResults.sbr"

"$(OUTDIR)\OleDb.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PgOleDb.pdb" /debug /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /pdbtype:sept /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\DebugU
TargetPath=.\DebugU\PgOleDb.dll
InputPath=.\DebugU\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"

OUTDIR=.\ReleaseMinSize
INTDIR=.\ReleaseMinSize
# Begin Custom Macros
OutDir=.\ReleaseMinSize
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" ".\OleDb.tlb" ".\OleDb.h" ".\OleDb_i.c" ".\ReleaseMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\ReleaseMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PgOleDb.pdb" /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\PgOleDb.dll
InputPath=.\ReleaseMinSize\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"

OUTDIR=.\ReleaseMinDependency
INTDIR=.\ReleaseMinDependency
# Begin Custom Macros
OutDir=.\ReleaseMinDependency
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" ".\OleDb.tlb" ".\OleDb.h" ".\OleDb_i.c" "$(OUTDIR)\OleDb.bsc" ".\ReleaseMinDependency\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\OleDb.sbr"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgCommand.sbr"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgDs.sbr"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgMultipleResults.sbr"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgRS.sbr"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\PgSess.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\TypeInfo.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\OleDb.bsc"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\ReleaseMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\OleDb.sbr" \
	"$(INTDIR)\PgCommand.sbr" \
	"$(INTDIR)\PgDs.sbr" \
	"$(INTDIR)\PgRS.sbr" \
	"$(INTDIR)\PgSess.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TypeInfo.sbr" \
	"$(INTDIR)\PgMultipleResults.sbr"

"$(OUTDIR)\OleDb.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PgOleDb.pdb" /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\PgOleDb.dll
InputPath=.\ReleaseMinDependency\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"

OUTDIR=.\ReleaseUMinSize
INTDIR=.\ReleaseUMinSize
# Begin Custom Macros
OutDir=.\ReleaseUMinSize
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" ".\OleDb.tlb" ".\OleDb.h" ".\OleDb_i.c" ".\ReleaseUMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\ReleaseUMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PgOleDb.pdb" /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\PgOleDb.dll
InputPath=.\ReleaseUMinSize\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"

OUTDIR=.\ReleaseUMinDependency
INTDIR=.\ReleaseUMinDependency
# Begin Custom Macros
OutDir=.\ReleaseUMinDependency
# End Custom Macros

ALL : "$(OUTDIR)\PgOleDb.dll" ".\OleDb.tlb" ".\OleDb.h" ".\OleDb_i.c" ".\ReleaseUMinDependency\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\OleDb.obj"
	-@erase "$(INTDIR)\OleDb.pch"
	-@erase "$(INTDIR)\OleDb.res"
	-@erase "$(INTDIR)\PgCommand.obj"
	-@erase "$(INTDIR)\PgDs.obj"
	-@erase "$(INTDIR)\PgMultipleResults.obj"
	-@erase "$(INTDIR)\PgRS.obj"
	-@erase "$(INTDIR)\PgSess.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TypeInfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PgOleDb.dll"
	-@erase "$(OUTDIR)\PgOleDb.exp"
	-@erase "$(OUTDIR)\PgOleDb.lib"
	-@erase ".\OleDb.h"
	-@erase ".\OleDb.tlb"
	-@erase ".\OleDb_i.c"
	-@erase ".\ReleaseUMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\OleDb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\OleDb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpqdll.lib Ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PgOleDb.pdb" /machine:I386 /def:".\OleDb.def" /out:"$(OUTDIR)\PgOleDb.dll" /implib:"$(OUTDIR)\PgOleDb.lib" /libpath:"$(PSQLSRC)\src\interfaces\libpq\Release" 
DEF_FILE= \
	".\OleDb.def"
LINK32_OBJS= \
	"$(INTDIR)\OleDb.obj" \
	"$(INTDIR)\PgCommand.obj" \
	"$(INTDIR)\PgDs.obj" \
	"$(INTDIR)\PgRS.obj" \
	"$(INTDIR)\PgSess.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TypeInfo.obj" \
	"$(INTDIR)\OleDb.res" \
	"$(INTDIR)\PgMultipleResults.obj"

"$(OUTDIR)\PgOleDb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\PgOleDb.dll
InputPath=.\ReleaseUMinDependency\PgOleDb.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL_PROJ=

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("OleDb.dep")
!INCLUDE "OleDb.dep"
!ELSE 
!MESSAGE Warning: cannot find "OleDb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "OleDb - Win32 Debug" || "$(CFG)" == "OleDb - Win32 Unicode Debug" || "$(CFG)" == "OleDb - Win32 Release MinSize" || "$(CFG)" == "OleDb - Win32 Release MinDependency" || "$(CFG)" == "OleDb - Win32 Unicode Release MinSize" || "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"
SOURCE=.\OleDb.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\OleDb.obj"	"$(INTDIR)\OleDb.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch" ".\OleDb_i.c" ".\OleDb.h"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\OleDb.obj"	"$(INTDIR)\OleDb.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\OleDb.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\OleDb.obj"	"$(INTDIR)\OleDb.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\OleDb.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\OleDb.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\OleDb.idl
MTL_SWITCHES=/tlb ".\OleDb.tlb" /h "OleDb.h" /iid "OleDb_i.c" /Oicf 

".\OleDb.tlb"	".\OleDb.h"	".\OleDb_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


SOURCE=.\OleDb.rc

"$(INTDIR)\OleDb.res" : $(SOURCE) "$(INTDIR)" ".\OleDb.tlb"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\PgCommand.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\PgCommand.obj"	"$(INTDIR)\PgCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch" ".\OleDb.h"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\PgCommand.obj"	"$(INTDIR)\PgCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\PgCommand.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\PgCommand.obj"	"$(INTDIR)\PgCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\PgCommand.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\PgCommand.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\PgDs.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\PgDs.obj"	"$(INTDIR)\PgDs.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch" ".\OleDb.h"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\PgDs.obj"	"$(INTDIR)\PgDs.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\PgDs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\PgDs.obj"	"$(INTDIR)\PgDs.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\PgDs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\PgDs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\PgMultipleResults.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\PgMultipleResults.obj"	"$(INTDIR)\PgMultipleResults.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\PgMultipleResults.obj"	"$(INTDIR)\PgMultipleResults.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\PgMultipleResults.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\PgMultipleResults.obj"	"$(INTDIR)\PgMultipleResults.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\PgMultipleResults.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\PgMultipleResults.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\PgRS.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\PgRS.obj"	"$(INTDIR)\PgRS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch" ".\OleDb.h"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\PgRS.obj"	"$(INTDIR)\PgRS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\PgRS.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\PgRS.obj"	"$(INTDIR)\PgRS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\PgRS.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\PgRS.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\PgSess.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\PgSess.obj"	"$(INTDIR)\PgSess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch" ".\OleDb.h"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\PgSess.obj"	"$(INTDIR)\PgSess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\PgSess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\PgSess.obj"	"$(INTDIR)\PgSess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\PgSess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\PgSess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /ZI /Od /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "$(PSQLSRC)\src\include" /I "$(PSQLSRC)\src\interfaces\libpq" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\OleDb.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\OleDb.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TypeInfo.cpp

!IF  "$(CFG)" == "OleDb - Win32 Debug"


"$(INTDIR)\TypeInfo.obj"	"$(INTDIR)\TypeInfo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Debug"


"$(INTDIR)\TypeInfo.obj"	"$(INTDIR)\TypeInfo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinSize"


"$(INTDIR)\TypeInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Release MinDependency"


"$(INTDIR)\TypeInfo.obj"	"$(INTDIR)\TypeInfo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinSize"


"$(INTDIR)\TypeInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ELSEIF  "$(CFG)" == "OleDb - Win32 Unicode Release MinDependency"


"$(INTDIR)\TypeInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\OleDb.pch"


!ENDIF 


!ENDIF 

