#!/usr/bin/env bash

if [ $# -lt 2 ]; then
    echo "Usage $0 <config file> <adhoc name>..."
fi

export CONFIG_FILE_PATH=$1
TEST=`find . -name $2`

echo "Executing adhoc case $TEST..."
$TEST
