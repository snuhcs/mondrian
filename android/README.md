# Mondrian Android Runtime

## Built With
* [Android Studio](https://developer.android.com/studio/)
* [TensorFlow Lite](https://www.tensorflow.org/lite/)
* [MNN](https://github.com/alibaba/MNN/)

## Prerequisites
1. Download dependencies into `Mondrian/src/main/jniLibs` and `Mondrian/src/main/cpp/third_party` from google drive.
2. Download models to use from google drive.
3. (Optional) Download source videos from google drive.

## Installation
1. Download the Android Studio.
2. Open current directory with the Android Studio.
3. Build and install the `Mondrian` application.

## How to Run
1. Build and install `Mondrian` application.
2. Upload `config.json` into `/data/local/tmp`.
    * `adb push <host config.json path> /data/local/tmp/config.json`
3. Upload models to use into `/data/local/tmp/models/`.
    * `adb push <host model path> /data/local/tmp/models`
4. Upload videos specified in `config.json`.
    * `adb push <host video path> <adb video path>`
5. Run `Mondrian`.

## Threads of the System
1. Video load threads (`VideoLoader.thread`)
  * JOB: Load video frame in yuv format
  * Input: Video
  * Output: ROIExtractor::PDWaiting_
2. PD extraction thread (`ROIExtractor::PDThread_`)
  * JOB: Extract PD ROIs
  * Input: ROIExtractor::PDWaiting_
  * Output: ROIExtractor::OFWaiting_
3. OF extraction thread (`ROIExtractor::OFThread_`)
  * JOB: Extract OF
  * Input: ROIExtractor::OFWaiting_
  * Output: ROIExtractor::PostprocessWaiting_
4. ROI Postprocessing thread (`ROIExtractor::PostprocessThread_`)
  * JOB: Filter, Scale, Merge
  * Input: ROIExtractor::PostprocessWaiting_
  * Output: ROIExtractor::Processed_
5. Schedule triggering and packing thread (`Mondrian::scheduleThread_`)
  * JOB: Get extracted frames and pack results
  * Input: ROIExtractor::Processed_
  * Output: Mondrian::packingResults_
6. Box reconstruction and interpolation thread (`Mondrian::postprocessThread_`)
  * JOB: Box reconstruction and interpolatoin
  * Input: Mondrian::packingResults_
  * Output: Mondrian::results_
7. Result logging thread (`Mondrian::logThread_`)
  * Input: Mondrian::results_
  * Output: Log files
