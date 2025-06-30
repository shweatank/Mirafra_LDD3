#!/usr/bin/env bash
cat $1 > /dev/null
if [ "$?" -eq 0 ]
then
	echo "File Exists!"
else
	echo "File Does not Exist!"
fi
