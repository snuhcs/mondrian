#!/bin/bash
dirname="$(date '+%Y-%m-%d_%H-%M-%S')_h$(openssl rand -hex 2)"

adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/roi.csv > csvs/roi.csv
r1=$?
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/exec.csv > csvs/exec.csv
r2=$?
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/boxes.txt > csvs/boxes.txt
r3=$?
if [ $r1 -eq 0 ] || [ $r2 -eq 0 ] || [ $r3 -eq 0]
then
    mkdir csvs/$dirname
    mv csvs/roi.csv csvs/$dirname/roi.csv
    mv csvs/exec.csv csvs/$dirname/exec.csv
    mv csvs/boxes.txt csvs/$dirname/boxes.txt
    echo "STATUS: roi.csv $r1 | exec.csv $r2 | boxes.txt $r3"
    echo "SAVED AT csvs/$dirname"
fi
