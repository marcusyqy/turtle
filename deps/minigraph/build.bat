@echo off
setlocal
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if not "%run%"=="1" set build=1

set debug_flags= /Od /D_DEBUG /MTd
set release_flags= /O2 /DNDEBUG /MT

set compile_flags=
set common_flags= /I..\ /nologo /MP /FC /Zi /std:c++17 /wd4530 /Zc:__cplusplus

if "%debug%"=="1" set compile_flags= %debug_flags% %common_flags%
if "%release%"=="1" set compile_flags= %release_flags% %common_flags%

if not exist build mkdir build


pushd build
for /f %%i in ('dir ..\examples /b /ad') do ( 
    if "%build%" == "1" call cl %compile_flags% ..\examples\%%i\*.cpp /Fe:%%i.exe
)
if "%run%" == "1" (
    for /f %%i in ('dir ..\examples /b /ad') do ( 
        echo [ RUNNING %%i ]
        call %%i.exe
        echo [ DONE %%i ]
    )
)
popd

@REM for %%a in (%*) do set "%%a=0"
set debug_flags=
set release_flags=
set compile_flags=
set common_flags=

