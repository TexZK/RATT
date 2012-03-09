#!/bin/bash
project_folder=sensor						# Matches the Eagle subfolder name

eagle_folder=UNKNOWN
this_folder=$(pwd)
platform=$(uname -a)
if [ "x${platform:0:5}" = "xMINGW" ]; then
	eagle_folder=$HOME/Documents/eagle		# Windows
else
	eagle_folder=$HOME/eagle				# Linux (default)
fi

cd "$eagle_folder/$project_folder"
files=$(ls -Q1 | grep -v '.*\..#.')
echo "$files" | xargs -i cp -Rv {} "$this_folder"
cd "$this_folder"
