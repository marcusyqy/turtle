@echo off
setlocal
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if "%~1"==""        echo [default mode]

set forward_flags=
if "%debug%"=="1"   set forward_flags=debug
if "%release%"=="1"   set forward_flags=release

:: build glfw
set build_glfw=
if "%glfw%"=="1" set build_glfw=build %forward_flags% && echo [BUILDING GLFW]
pushd deps\glfw
call %build_glfw%
popd

:: build imgui
set build_imgui=
if "%imgui%"=="1" set build_imgui=build %forward_flags% && echo [BUILDING IMGUI]
pushd deps\imgui
call %build_imgui%
popd

set debug_flags= /Od /D_DEBUG /MTd
set release_flags= /O2 /DNDEBUG /MT

set compile_flags=
set include_deps= /I..\deps\imgui\imgui /I..\deps\glfw\include /I..\deps\minigraph /I..\deps\glfw\deps
set common_flags= %include_deps% /I..\src\ /nologo /MP /FC /Zi /Zc:__cplusplus /std:c++17 /wd4530 /utf-8

if "%debug%"=="1" set compile_flags= %debug_flags% %common_flags%
if "%release%"=="1" set compile_flags= %release_flags% %common_flags%
set glfw_link= ..\deps\glfw\lib\glfw.lib
set imgui_link= ..\deps\imgui\lib\imgui.lib
set links= /link %imgui_link% %glfw_link% opengl32.lib gdi32.lib shell32.lib

:: build turtle
if not exist build mkdir build
pushd build
call cl %compile_flags% ..\src\turtle_main.cpp ..\src\backends\*.cpp /Fe:turtle.exe %links%
popd

for %%a in (%*) do set "%%a=0"
set debug=
set release=
set compile_flags=
set common_flags=

