#!/bin/sh

OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=cc
CALL=sh

# declare variables
debug=0
release=0

[ ! -d "lib" ] && mkdir lib

# set variables to
for arg in "$@"; do
    eval "$arg=1"
done

echo COMPILER:$COMPILER

CFLAGS=
if [ "$release" -eq "1" ]; then
   echo "Release enabled"
else
    eval "debug=1"
fi

[ "$debug" -eq "1" ] && echo "Debug enabled" && eval "CFLAGS=-D_DEBUG"

cd lib
$COMPILER $CFLAGS -c ../glfw_common.c -o glfw_common.o
$COMPILER $CFLAGS -c ../glfw_x11.c -o glfw_x11.o -D_GLFW_X11
ar rvs glfw.a glfw_common.o glfw_x11.o

# end with reverting back
cd $OLDPWD

