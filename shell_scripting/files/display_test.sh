#!/bin/bash
file="myfile.txt"
if [ -f "$file" ]; then
    echo "Contents of $file:"
    cat "$file"
else
    echo "File not found!"
fi

