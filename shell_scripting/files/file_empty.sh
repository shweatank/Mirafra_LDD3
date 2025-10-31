#!/bin/bash
file="file1.txt"
if [ -s "$file" ]; then
	echo "file not empty"
else
	echo "file is empty"
fi
