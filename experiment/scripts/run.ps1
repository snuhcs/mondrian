Set-PSDebug -Trace 1

$CONFIG_PATH="/data/local/tmp/edgeserver.json"
$APP_DATA_PATH="/data/data/hcs.offloading.edgeserver"

# adb push  ./experiment/configs/end_to_end/e2e_0_full_inference.json           $CONFIG_PATH
# adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
# Start-Sleep -Seconds 30
# adb shell am force-stop hcs.offloading.edgeserver
# adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/e2e_0_full_inference.log > ./results/e2e_0_full_inference.log
# Start-Sleep -Seconds 20


adb push  ./experiment/configs/end_to_end/e2e_1_accuracy_sacrificing.json     $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/e2e_1_accuracy_sacrificing.log > ./results/e2e_1_accuracy_sacrificing.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/end_to_end/e2e_2_throughput_sacrificing.json   $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/e2e_2_throughput_sacrificing.log > ./results/e2e_2_throughput_sacrificing.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/end_to_end/e2e_3_ours.json                     $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/e2e_3_ours.log > ./results/e2e_3_ours.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/performance_breakdown/pb_0_baseline.json       $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/pb_0_baseline.log > ./results/pb_0_baseline.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/performance_breakdown/pb_1_roiprop.json        $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/pb_1_roiprop.log > ./results/pb_1_roiprop.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/performance_breakdown/pb_2_roiprop_mixing.json $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/pb_2_roiprop_mixing.log > ./results/pb_2_roiprop_mixing.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/roi_extraction_method/rem_0_pd.json            $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/rem_0_pd.log > ./results/rem_0_pd.log
Start-Sleep -Seconds 20


adb push  ./experiment/configs/roi_extraction_method/rem_1_of.json            $CONFIG_PATH
adb shell am start-activity hcs.offloading.edgeserver/.MainActivity
Start-Sleep -Seconds 30
adb shell am force-stop hcs.offloading.edgeserver
adb shell run-as hcs.offloading.edgeserver cat $APP_DATA_PATH/rem_1_of.log > ./results/rem_1_of.log
# Start-Sleep -Seconds 20


