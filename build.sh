#!/bin/sh
OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=c++
PLATFORM=$(uname -s)
TARGET=DEBUG

COMMON_FLAGS="-g -std=c++17"
DEBUG_FLAGS="-O0 -D_DEBUG"
RELEASE_FLAGS="-O2"
INCLUDE_FLAGS="-I../deps/imgui/imgui -I../deps/glfw/include -I../deps/minigraph -I../deps/glfw/deps -IX11"
LINK_FLAGS="../deps/glfw/lib/glfw.a ../deps/imgui/lib/imgui.a -lGL -lX11 -ldl -pthread"


# declare variables
debug=0
release=0
format=0
glfw=0
imgui=0
turtle=0
all=0

[ -z "$@" ] && all=1

[ ! -d "build" ] && mkdir build

# set variables to
for arg in "$@"; do
    eval "$arg=1"
done

if [ "$release" -eq "1" ]; then
   eval "TARGET_FLAGS=\"$RELEASE_FLAGS\""
   eval "TARGET=DEBUG"
else
    eval "debug=1"
fi

[ "$debug" -eq "1" ] && eval "TARGET=DEBUG" && eval "TARGET_FLAGS=\"$DEBUG_FLAGS\""


echo COMPILER:$COMPILER
echo TARGET:$TARGET
echo PLATFORM:$PLATFORM

if [ "$all" -eq "1" ]; then
    eval "imgui=1"
    eval "turtle=1"
    eval "glfw=1"
fi

if [ "$glfw" -eq "1" ]; then
    echo "BUILD glfw"
    sh "deps/glfw/build.sh" "$@"
fi

if [ "$imgui" -eq "1" ]; then
    echo "BUILD IMGUI"
    sh "deps/imgui/build.sh" "$@"
fi

if [ "$turtle" -eq "1" ]; then
    echo "BUILD TURTLE"
    cd build
    $COMPILER $COMMON_FLAGS $TARGET_FLAGS $INCLUDE_FLAGS ../src/turtle_main.cpp ../src/backends/*.cpp $LINK_FLAGS -o turtle
fi


# end with reverting back
cd $OLDPWD
