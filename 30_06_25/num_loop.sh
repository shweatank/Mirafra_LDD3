#!/bin/bash
num=1
while [ $num -le 20 ]
do

        if [ $num -le 10 ]
        then
	    echo "num=$num"
        else
            echo "num=$num"
	    echo "Iam the big"
        fi
	((num++))
done
