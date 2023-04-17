#!/bin/bash
echo ""

device_num=$(($(adb devices | wc -l) - 2))

if [ $device_num -lt 1 ]
then
    echo "### NO DEVICE FOUND"
else
    echo "### CONFIG UPDATE START"
    echo ""
    adb push mondrian.json /data/local/tmp/mondrian.json
    echo ""
    echo "### CONFIG UPDATE DONE"
fi
echo ""
