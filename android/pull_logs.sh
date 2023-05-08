#!/bin/bash
base="logs"
dir="$(date '+%Y-%m-%d_%H-%M-%S')_h$(openssl rand -hex 2)"

adb shell cat /data/local/tmp/config.json > $base/config.json
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/roi.csv > $base/roi.csv
r1=$?
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/timeline.csv > $base/timeline.csv
r2=$?
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/boxes.txt > $base/boxes.txt
r3=$?

mkdir -p $base/$dir
mv $base/config.json $base/$dir/config.json

if [ $r1 -eq 0 ]
then
    mv $base/roi.csv $base/$dir/roi.csv
fi

if [ $r2 -eq 0 ]
then
    mv $base/timeline.csv $base/$dir/timeline.csv
fi

if [ $r3 -eq 0 ]
then
    mv $base/boxes.txt $base/$dir/boxes.txt
fi

if [ $r1 -eq 0 ] || [ $r2 -eq 0 ] || [ $r3 -eq 0 ]
then
    echo "status : roi.csv $r1 | exec.csv $r2 | boxes.txt $r3"
    echo "saved at $base/$dir"
fi
