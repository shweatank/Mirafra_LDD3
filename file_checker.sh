#!/bin/bash

# Check if a filename is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

# Try to cat the file but suppress output
cat "$1" > /dev/null 2>&1

# Check the exit status of the cat command
if [ $? -eq 0 ]; then
    echo "File exists!"
else
    echo "File does not exist!"
fi

