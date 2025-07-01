#!/bin/bash

cat "$1"

if [ $? -eq 0 ]
then
    echo "File exists!"
else
    echo "File does not exist!"
fi
