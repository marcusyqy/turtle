#!/bin/sh

OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=c++
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
$COMPILER $CFLAGS -I../imgui/ -c ../imgui_lib.cpp -o imgui_lib.o
ar rvs imgui.a imgui_lib.o

# end with reverting back
cd $OLDPWD


