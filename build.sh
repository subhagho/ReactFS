#!/bin/bash

CMAKE="/Applications/CMake.app/Contents/bin/cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
#CMAKE="cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"

DIRS="common watergate core"
DIRS_TO_BUILD=""

for dir in $DIRS;
do
    DIRS_TO_BUILD="$DIRS_TO_BUILD $dir"
    if [ -d "$dir/test" ];then
        DIRS_TO_BUILD="$DIRS_TO_BUILD $dir/test"
	fi
done

echo "Building directories [$DIRS_TO_BUILD] ..."

ROOTDIR=$PWD

for dir in $DIRS_TO_BUILD;
do
    if [ ! -d "$dir/cmake" ]; then
	    cd $dir
	    echo "Creating make directory [$dir/cmake]..."
	    mkdir cmake
	    cd -
    fi
    cd $dir/cmake
	echo "Current directory $PWD..."
	echo "Generating makefiles..."
	$CMAKE -G"Unix Makefiles" ..

	echo "Building $PWD..."
	make -f Makefile

    cd $ROOTDIR
done

echo "Build completed..."



