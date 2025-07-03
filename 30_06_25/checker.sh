#!/bin/bash

# Take filename from the first argument
file_name=$1

# Run cat (this will output the file content if it exists)
cat "$file_name" > /dev/null 2>&1

# Check the exit status of cat using $?
if [ $? -eq 0 ]; then
    echo "File exists!"
else
    echo "File does not exist!"
fi

