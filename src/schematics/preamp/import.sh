#!/bin/bash
project_folder=preamp					# Matches the Eagle subfolder name

eagle_folder=UNKNOWN
this_folder=$(pwd)
platform=$(uname -a)
if [ "x${platform:0:5}" = "xMINGW" ]; then
	eagle_folder=$HOME/Documents/eagle		# Windows
else
	eagle_folder=$HOME/eagle			# Linux (default)
fi

source=$eagle_folder/$project_folder
target=$this_folder

files=$(ls -1 "$source" | grep -v '.*\..#.')
echo "$files" | xargs -i cp -Rv "$source/{}" "$target"

