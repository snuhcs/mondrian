adb pull /data/local/tmp/config.json
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/roi.csv > roi.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/timeline.csv > timeline.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/boxes.csv > boxes.csv
