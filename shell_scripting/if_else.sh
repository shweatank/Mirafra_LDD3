#!/bin/bash
echo "enter num:"
read num
if [ $num -gt 10 ]; then
	echo "num greater than 10"
else
	echo "num less than or eq to 10"
fi
