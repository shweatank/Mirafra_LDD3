#!/bin/bash
echo "enter n value:"
read n
i=1
while [ $i -le $n ]
do
	echo "$i "
	i=$((i+1))
done
