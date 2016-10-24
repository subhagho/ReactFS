#!/usr/bin/env bash

if [ $# -lt 2 ]; then
    echo "Usage $0 <config file> <test name>..."
fi

export CONFIG_FILE_PATH=$1

./cmake/$2