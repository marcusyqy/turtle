#!/bin/sh
OLDPWD=$PWD
cd "${0%/*}" # cd to where the script is

COMPILER=c++

CALL=sh

[ ! -d "build" ] && mkdir build

echo $CALL
echo $COMPILER
echo $PWD

cd build
cd $OLDPWD
