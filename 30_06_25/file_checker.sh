#!/bin/bash
file_name=$1
if [ -f "$file_name" ];
then 
	cat "$file_name"
	echo "file exsists"
else
	echo "file doesnot exist"
fi
