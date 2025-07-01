#!/usr/bin/env bash
greeting="Hello world!"
echo $greeting
echo '$greeting'
echo "$greeting"
x=foo
echo '$x'
num=101
if [ $num -gt 100 ]
then
	echo "That's a big number!"
fi

num=101
if [ $num -gt 100 ] && [ $num -lt 1000 ]
then
	echo "that's a big (but not a too big) number!"
fi
