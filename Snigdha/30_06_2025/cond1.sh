#!/bin/bash

num=1001
if [ $num -gt 1000 ]
then
	echo "That's a big number"
elif [ $num -gt 100 ] && [ $num -lt 1000 ]
then 
	echo "That is median number"
else
	echo "That's small number"
fi
