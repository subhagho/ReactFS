#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    # Do something under Mac OS X platform
    CMAKE="/Applications/CMake.app/Contents/bin/cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Do something under GNU/Linux platform
    CMAKE="cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
    if [ -f ./.env ]; then
	echo "Setting up local environemnt..."
	. ./.env
    fi
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    # Do something under Windows NT platform
    echo "cmake not defined..."
    exit -1
fi


echo "Using makefiles [$CMAKE]..."

DIRS="common watergate parsers core"
DIRS_TO_BUILD=""

for dir in $DIRS;
do
    DIRS_TO_BUILD="$DIRS_TO_BUILD $dir"
    if [ -d "$dir/test" ];then
        if [ -f "$dir/test/CMakeLists.txt" ]; then
            DIRS_TO_BUILD="$DIRS_TO_BUILD $dir/test"
        fi
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
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

	echo "Building $PWD..."
	make -f Makefile
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

    cd $ROOTDIR
done

echo "Build completed..."



