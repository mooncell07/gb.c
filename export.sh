#!/bin/bash

packer="poetry"
formatter="clang-format"
header="#define __AUTOGEN__"
filepath=$(readlink -f "$1")

if ! command -v "$packer" > /dev/null; then
  echo "Poetry not found."
  exit 1
fi

if [ $# -eq 0 ]; then
  echo "Pass a filepath."
  exit 1
fi

if [[ -d $filepath ]]; then
  echo "$filepath is a directory."
  exit 21
elif [[ -f $filepath ]]; then
  echo "File has been located. Checking if its safe to overwrite this file."
  read -r file_header < "$filepath"
  if [[ $header == $file_header ]]; then
    echo "File safe to overwrite. Proceeding now."
  else
    echo "File cant be safely modified. The autogen header is missing. Perhaps you passed path to the wrong file? If you think this is not true then manually add #define __AUTOGEN__ on first line."
    exit 1
  fi
else
  echo "Bad Argument."
  exit 22
fi

cd ./codegen/

packer+=" run python codegen"
output=$(eval "$packer")
echo "$output" > "$filepath"

echo "Contents have been written to $filepath"

if ! type "${formatter}" > /dev/null; then
  echo "Formatter ($formatter) not found. It is highly recommended to format the code. Preferably using clang-format (google style)"
  exit 1
fi

clang-format -i "$filepath"
echo "Code has been formatted. Exiting."
exit 0
