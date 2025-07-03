#!/bin/bash
create_floder_and_file()
{
	folder_name=$1
	file_name=$2
	mkdir -p "$folder_name"
        touch "$folder_name/$file_name"
	echo "File is created and writing the text into it"> "$folder_name/$file_name"
	echo "Created folder '$folder_name' and file '$file_name' inside it."
}
create_floder_and_file "$1" "$2"
