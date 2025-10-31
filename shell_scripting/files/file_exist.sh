#!/bin/bash
file="file1.txt"
#if [ -e "$file" ]; then
if [ -f "$file" ]; then
	echo "file exist"
else
	echo "file not exist"
fi
