#!/usr/bin/env bash
fun(){
	cat $1
	if [ $? -eq 0 ];then
		echo "file exists"
	else
		echo "file not exist"
	fi
}
fun "revathi.txt"
#$? gives exit status of last cmd executed
