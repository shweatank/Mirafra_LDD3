#!/usr/bin/env bash

if [ "$#" -ne 2 ]; then 
	echo "Usage: $0 <folder_name> <file_name.txt>"
	exit 1
fi

folder_name="$1"
file_name="$2"

create_folder_and_file(){
	mkdir -p "$folder_name"
	touch "$folder_name/$file_name"
	echo "created folder '$folder_name' and file '$file_name' inside it"
}
create_folder_and_file 

