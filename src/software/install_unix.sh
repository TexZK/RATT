#!/bin/sh
vdjFolder="$HOME/Documents/VirtualDJ/Device"
deviceFolder="$vdjFolder/Devices"
mapperFolder="$vdjFolder/Mappers"
mkdir -p "$deviceFolder"
mkdir -p "$mapperFolder"
cp -v "ratt_device.xml" "$deviceFolder/"
cp -v "ratt_mapping.xml" "$mapperFolder/"
