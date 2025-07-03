#!/bin/bash
num=101
if [ $num -gt 1000 ]
then
	echo "thats a huge num"
elif [ $num -gt 100 ]
then
	echo "thats a big number"
else
	echo "thas a small number"
fi
