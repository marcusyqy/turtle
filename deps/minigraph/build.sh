#!/bin/sh

OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=c++
CALL=sh

# declare variables
debug=0
release=0
build=0
run=0

[ ! -d "build" ] && mkdir build

# set variables to
for arg in "$@"; do
    eval "$arg=1"
done

if [ ! "$run" -eq "1" ]; then
    build=1
fi

echo COMPILER:$COMPILER

COMMON_FLAGS="-g -std=c++17"
DEBUG_FLAGS="-O0 -D_DEBUG"
RELEASE_FLAGS="-O2"

if [ "$release" -eq "1" ]; then
   echo "Release enabled"
   eval "TARGET_FLAGS=\"$RELEASE_FLAGS\""
else
    eval "debug=1"
fi

[ "$debug" -eq "1" ] && echo "Debug enabled" && eval "TARGET_FLAGS=\"$DEBUG_FLAGS\""


directories=
if [ -d "examples" ]; then
    cd examples
    directories=$(ls)
    cd .. 
fi

cd build

if [ "$build" -eq "1" ]; then
    for arg in ${directories}; do
        $COMPILER $COMMON_FLAGS $TARGET_FLAGS -I.. ../examples/$arg/*.cpp -o $arg
    done
fi

if [ "$run" -eq "1" ]; then
    echo [RUNNING]
    for arg in ${directories}; do
        ./$arg
    done
fi

# end with reverting back
cd $OLDPWD


