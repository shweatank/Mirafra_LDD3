#!/usr/bin/env bash
#x=10
#y=10
echo "enter x"
read x
echo "enter y"
read y
#z=`expr $x + $y `
z=`echo $x+$y | bc`
echo "result is "$z
