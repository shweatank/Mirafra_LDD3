#!/bin/bash

FOLDER_NAME=$1
FILE_NAME=$2

create() {
    mkdir -p "$FOLDER_NAME"             
    touch "$FOLDER_NAME/$FILE_NAME"     
}

create

echo "folder is '$FOLDER_NAME' and file '$FILE_NAME' is created."

