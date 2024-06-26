cd /d %~dp0
@ECHO off
SETLOCAL enabledelayedexpansion
cls
COLOR 1f

If Defined SOUI4PATH (
Echo %SOUI4PATH%
) Else (
Echo can't find env variable SOUI4PATH, clone soui core and install wizard first, please.
goto final
)

SET cfg=

set file=%SOUI4PATH%\config\build.cfg
for /f "tokens=1,2* delims==" %%i in (%file%) do (
if "%%i"=="UNICODE" set cfg_unicode=%%j
if "%%i"=="WCHAR" set cfg_wchar=%%j
if "%%i"=="MT" set cfg_mt=%%j
)

if %cfg_mt%==1 ( SET cfg=%cfg% USING_MT)
if %cfg_unicode%==0 (SET cfg=%cfg% MBCS)
if %cfg_wchar%==0 (SET cfg=%cfg% DISABLE_WCHAR)

SET specs=
SET selected=
SET vsvarbat=
SET target=

rem 选择编译版本
SET /p selected=1.选择编译版本[1=x86;2=x64]:
if %selected%==1 (
	SET target=x86
) else if %selected%==2 (
	SET target=x64
	SET cfg=!cfg! x64
) else (
	goto error
)



SET proj_ext=

rem 选择开发环境
SET /p selected=2.选择开发环境[1=2008;2=2010;3=2012;4=2013;5=2015;6=2017;7=2005;8=2019;9=2022]:

for /f "skip=2 delims=: tokens=1,*" %%i in ('%windir%\system32\reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion" /v "ProgramFilesDir (x86)"') do ( 
	    set str=%%i
		set var=%%j
		set "var=!var:"=!"
		if not "!var:~-1!"=="=" set strCMD=!str:~-1!:!var!
 )
SET strCMD=%strCMD%\Microsoft Visual Studio\Installer\vswhere.exe


rem 通过开发环境判断版本
SET verSion=[16,17]
if %selected%==9 (		 
	SET verSion=[17,18]
)
if exist "%strCMD%" (
	for /f "delims=" %%i in ('"%strCMD%" -nologo -version !verSion! -prerelease -property installationPath -format value') do (
		set vs2019path=%%i
	)
)


if %selected%==1 (
	SET specs=win32-msvc2008
	SET proj_ext=vcproj
	SET vsvarbat="!VS90COMNTOOLS!..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==2 (
	SET specs=win32-msvc2010
	SET proj_ext=vcxproj
	SET vsvarbat="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==3 (
	SET specs=win32-msvc2012	
	SET proj_ext=vcxproj
	SET vsvarbat="%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==4 (
	SET specs=win32-msvc2013
	SET proj_ext=vcxproj
	SET vsvarbat="%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==5 (
	SET specs=win32-msvc2015
	SET proj_ext=vcxproj
	SET vsvarbat="%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
)else if %selected%==6 (
	SET specs=win32-msvc2017
	SET proj_ext=vcxproj
	for /f "skip=2 delims=: tokens=1,*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "15.0" /reg:32') do ( 
	    set str=%%i
	    set var=%%j
	    set "var=!var:"=!"
	    if not "!var:~-1!"=="=" set value=!str:~-1!:!var!
    )
    SET value=!value!\VC\Auxiliary\Build\vcvarsall.bat
    rem ECHO Vs2017 path is:!value! 
	SET vsvarbat="!value!"
	call !vsvarbat! %target%
	rem call "!value!" %target%
	goto built
) else if %selected%==7 (
	SET specs=win32-msvc2005
	SET proj_ext=vcproj
	SET vsvarbat="%VS80COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS80COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==8 (
	SET specs=win32-msvc2017
	SET proj_ext=vcproj
	SET value=!vs2019path!\VC\Auxiliary\Build\vcvarsall.bat
	ECHO Vs2019 path is:!value! 
	SET vsvarbat="!value!"
	call !vsvarbat! %target%
	rem call "%VS80COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==9 (
	SET specs=win32-msvc2017
	SET proj_ext=vcproj
	SET value=!vs2019path!\VC\Auxiliary\Build\vcvarsall.bat
	ECHO Vs2019 path is:!value! 
	SET vsvarbat="!value!"
	call !vsvarbat! %target%
	rem call "%VS80COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else (
	goto error
)

:built

if %specs%==win32-msvc2017 (	
	%SOUI4PATH%\tools\qmake2017 -tp vc -r -spec %SOUI4PATH%\tools\mkspecs\%specs% "CONFIG += !cfg! "
) else (
	%SOUI4PATH%\tools\qmake -tp vc -r -spec %SOUI4PATH%\tools\mkspecs\%specs% "CONFIG += !cfg! "
)

rem call devenv souitest.%proj_ext%

goto :eof

:error
echo "error found"
