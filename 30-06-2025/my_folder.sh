#!/bin/bash

# Function to create folder and file
create_folder_and_file() {
  # Create folder
  mkdir "$1"
  
  # Create file inside folder
  touch "$1/$2"
}

# Check if two arguments are provided
if [ $# -ne 2 ]; then
  echo "Error: Two arguments are required."
  exit 1
fi

# Call the function with the provided arguments
create_folder_and_file "$1" "$2"
