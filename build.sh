#!/bin/sh
OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=c++
PLATFORM=$(uname -s)
TARGET=DEBUG

# declare variables
debug=0
release=0
format=0
glfw=0
imgui=0
turtle=0


[ ! -d "build" ] && mkdir build

# set variables to
for arg in "$@"; do
    eval "$arg=1"
done

# set target
[ "$debug" -eq "1" ] && eval "TARGET=DEBUG"
[ "$release" -eq "1" ] && eval "TARGET=RELEASE"

echo COMPILER:$COMPILER
echo TARGET:$TARGET
echo PLATFORM:$PLATFORM

if [ "$imgui" -eq "1" ]; then
    echo "BUILD IMGUI"
fi

if [ "$glfw" -eq "1" ]; then
    echo "BUILD glfw"
    #sh "deps/glfw/"
fi

if [ "$turtle" -eq "1" ]; then
    echo "BUILD TURTLE"
    cd build
fi


# end with reverting back
cd $OLDPWD
