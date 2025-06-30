#!/usr/bin/env bash
num=0
while [ $num -lt 20 ]
do
if [ $num -gt 10 ]
then
	echo "I'm big"
else 
	echo $num
fi
num=$((num+1))


done
