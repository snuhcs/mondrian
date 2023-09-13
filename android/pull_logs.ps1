adb pull /data/local/tmp/config.json
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/boxes.csv > boxes.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/roi.csv > roi.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/frame.csv > frame.csv
adb shell run-as hcs.offloading.mondrian cat /data/data/hcs.offloading.mondrian/trace.json > trace.json
