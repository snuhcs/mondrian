#!/bin/bash

adb pull /data/local/tmp/config.json
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/roi.csv > roi.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/timeline.csv > timeline.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/boxes.txt > boxes.txt

mkdir $1
mv config.json roi.csv timeline.csv boxes.txt $1

python3.8 eval_file.py -p -d $1
