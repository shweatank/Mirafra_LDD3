#!/bin/bash

# Check if a file name is provided as an argument
if [ $# -eq 0 ]; then
  echo "Error: Please provide a file name as an argument."
  exit 1
fi

# Check if file exists and is readable
if [ -f "$1" ] && [ -r "$1" ]; then           # -f "$1" true if it is a regular file    and   -r "$1" true if it is readable
  echo "File exists and is readable!"
else
  echo "File does not exist or is not readable!"
fi

