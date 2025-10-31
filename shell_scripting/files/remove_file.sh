#!/bin/bash
file="dest.txt"
if [ -f "$file" ]; then
    rm "$file"
    echo "$file deleted successfully!"
else
    echo "File not found!"
fi

