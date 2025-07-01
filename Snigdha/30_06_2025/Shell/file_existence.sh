#!/bin/sh 


if [ $# -ne 1 ]
then
	echo "Usage: $0 <file_name>"
	exit 1
fi

cat "$1"

status=$?

if [ $status -eq 0 ]
then 
	echo "File '$1' exists"
else
	echo "File '$1' doesn't exists"
fi
