#!/bin/bash
file_name=$1
cat "$file_name" > /dev/null 2>&1
if [ $? -eq 0 ]
then
	echo "File exists!"
else
	echo "File does not exist!"
fi
