#!/bin/bash
echo ""

device_num=$(($(adb devices | wc -l) - 2))

if [ $device_num -lt 1 ]
then
    echo "### NO DEVICE FOUND"
else
    echo "### CONFIG UPDATE START"
    echo ""
    adb push strm.json /data/local/tmp/strm.json
    echo ""
    echo "### CONFIG UPDATE DONE"
fi
echo ""
