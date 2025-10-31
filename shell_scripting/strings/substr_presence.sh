#!/bin/bash
str1="i am swarna latha"
if [[ "$str1" == *"swarna"* ]];then
	echo "yes substr present"
else
	echo "no not present"
fi
