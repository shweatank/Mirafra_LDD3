#!/bin/bash
echo "enter file name:"
read filename
if [ -f $filename ]; then
	echo "file $filename exist"
else
	echo "file $filename not exist"
fi
