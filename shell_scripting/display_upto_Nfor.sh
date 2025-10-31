#!/bin/bash
i=1
echo "enter n:"
read n
#for i in $(seq 1 $n)
for (( i;i<=n;i++ ))
do
	echo "$i"
done
