#!/bin/bash
project_folder=sensor					# Matches the Eagle subfolder name

eagle_folder=UNKNOWN
this_folder=$(pwd)
platform=$(uname -a)
if [ "x${platform:0:5}" = "xMINGW" ]; then
	eagle_folder=$HOME/Documents/eagle		# Windows
else
	eagle_folder=$HOME/eagle			# Linux (default)
fi

source=$this_folder
target=$eagle_folder/$project_folder
files=$(ls -1 "$this_folder" | grep -v \.sh$)

rm -rf "$target"
mkdir -p "$target"
echo "$files" | xargs -i cp -Rv {} "$target"

