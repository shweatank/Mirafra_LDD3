#!/bin/sh 


create_folder_file()
{
	folder_name="$1"
	file_name="$2"

	mkdir -p "$folder_name"
	touch "$folder_name/$file_name"

	echo "Folder '$folder_name' with file '$file_name' has successfully created"
}

if [ "$#" -ne 2 ]
then
	echo "Usage : $0 <your_name><your_name.txt>"
	exit 1
fi

create_folder_file "$1" "$2"

