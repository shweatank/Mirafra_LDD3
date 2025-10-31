#!/bin/bash
echo "enter dir name"
read dir
if [ -d $dir ]; then
	count=$(ls $dir | wc -l)
	echo "number of files:$count"
else
	echo "directory not found"
fi
