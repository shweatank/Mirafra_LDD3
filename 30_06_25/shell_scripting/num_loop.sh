#!/bin/bash
num=0
for i in {1..20}
do
	echo $num
	num=$((num+1))
	if [ $num -gt 10 ]
	then
		echo "I am big!"
	fi
done
