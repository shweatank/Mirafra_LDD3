#!/bin/bash

# my_folder.sh - Create a folder and a file inside it

# Function to create folder and file
create_folder_and_file() {
  folder_name="$1"
  file_name="$2"

  # Create the folder if it doesn't exist
  mkdir -p "$folder_name"

  # Create the file inside the folder
  touch "$folder_name/$file_name"

 
  echo "Created file: $folder_name/$file_name"
}

# Check for two arguments
if [ $# -ne 2 ]; then
  echo "Usage: $0 <folder_name> <file_name.txt>"
  exit 1
fi

# Call the function with arguments
create_folder_and_file "$1" "$2"
