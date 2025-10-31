#!/bin/bash
file="myfile.txt"
if [ -f "$file" ]; then
    wc "$file"
else
    echo "File not found!"
fi

