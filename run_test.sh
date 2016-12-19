#!/usr/bin/env bash

if [ $# -lt 2 ]; then
    echo "Usage $0 <test name> <config file> ..."
fi

export CONFIG_FILE_PATH=$2
TEST=$1

echo "Executing adhoc case $TEST..."
$TEST
