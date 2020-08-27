@ECHO OFF

REM INITIALIZE THE COMPILER ENVIRONMENT.
WHERE cl.exe
REM IF %ERRORLEVEL% NEQ 0 CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
WHERE cl.exe

SET TOOL_DIRECTORY_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.25.28610\bin\Hostx64\x64"
SET COMPILER_PATH="%TOOL_DIRECTORY_PATH%\cl.exe"
SET LIB_TOOL_PATH="%TOOL_DIRECTORY_PATH%\lib.exe"

REM DEFINE COMPILER OPTIONS.
REM /W4 
SET COMMON_COMPILER_OPTIONS=/EHsc /TP /std:c++latest
SET DEBUG_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /Z7 /Od /MTd
SET RELEASE_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /O2 /MT

SET INCLUDE_DIRS=/I "..\code" /I "..\ThirdParty"

SET LIBRARY_DIRS=/LIBPATH:"..\ThirdParty\raylib"

SET LIBRARIES=user32.lib gdi32.lib winmm.lib raylib.lib

SET COMPILATION_FILE="..\RaylibDemo.project"

REM MOVE INTO THE BUILD DIRECTORY.
IF NOT EXIST "build" MKDIR "build"
PUSHD "build"
    "%COMPILER_PATH%" %DEBUG_COMPILER_OPTIONS% %COMPILATION_FILE% %INCLUDE_DIRS% /link %LIBRARIES% %LIBRARY_DIRS%
    REM /NODEFAULTLIB:MSVCRT

    IF %ERRORLEVEL% EQU 0 (
        COPY "..\ThirdParty\raylib\raylib.dll" raylib.dll
    )
POPD

ECHO Done

@ECHO ON