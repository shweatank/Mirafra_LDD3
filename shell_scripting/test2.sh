#sh test2.sh 102

#!/usr/bin/env bash
num=$1
if [ $num -gt 100 ]
then
	echo "thats a big number"
fi
