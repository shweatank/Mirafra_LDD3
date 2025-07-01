#!/bin/bash
func()
{
	folder_name=$1
	file_name=$2
	mkdir -p "$folder_name"
	touch "$folder_name/$file_name"
	echo $folder_name "and" $file_name "created"
}
func "$1" "$2"
