#!/bin/bash

# file_checker.sh - Check if a file exists using cat and exit status

# Check if argument is provided
if [ $# -ne 1 ]; then
  echo "Usage: $0 <pritesh>"
  exit 1
fi

# Try to cat the file, suppress output
cat "$1" > /dev/null 2>&1

# Check the exit status
if [ $? -eq 0 ]; then
  echo "File exists!"
else
  echo "File does not exist!"
fi
