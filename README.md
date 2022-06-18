<div id="top"></div>

# Multi-Camera Spatio-temporal RoI Mixing
FYC, [STRMCPP/src/main/cpp/strm](https://github.com/snuhcs/offloading-system/tree/master/STRMCPP/src/main/cpp/strm)
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#citation">Citation</a></li>
  </ol>
</details>


## About The Project
This project enables effective multi-stream video analytics via spatio-temporal packing.

### Built With
* [TensorFlow Lite](https://www.tensorflow.org/lite/)
* [MNN](https://github.com/alibaba/MNN/)
* [WebRTC](https://webrtc.org/)
* [MQTT](https://mqtt.org/)
* [Android Studio](https://developer.android.com/studio/)


## Getting Started (C++)

### Prerequisites
1. Download dependencies into `STRMCPP/src/main/jniLibs` and `STRMCPP/src/main/cpp/third_party` from google drive.
2. Download models to use from google drive.
3. (Optional) Download source videos from google drive.

### Installation
1. Download the Android Studio.
2. Open this project with the Android Studio.
3. Select each application to install. (`EdgeDeviceCPP`, `SourceCamera`, `EdgeHub`)
4. Build and install the target application.

### How to Run
#### Option 1. Process videos on the EdgeDevice
1. Build and install `EdgeDeviceCPP` application.
2. Upload `edgedevicecpp.json` and `strmcpp.json` into `/data/local/tmp`.
    * `adb push <host edgedevicecpp.json path> /data/local/tmp/edgedevicecpp.json`
    * `adb push <host strmcpp.json path> /data/local/tmp/strmcpp.json`
3. Upload models to use into `/data/local/tmp/models/`.
    * `adb push <host model path> /data/local/tmp/models`
4. (Only for `Option 1. Process videos on the EdgeDevice`) Upload videos specified in `edgedevicecpp.json`.
    * `adb push <host video path> <adb video path>`
5. Run `EdgeDeviceCPP`.

#### Option 2. Process videos on SourceCameras
1. Build and install `EdgeDeviceCPP`, `SoureCamera`, and `EdgeHub` to different mobile devices.
2. Setup `EdgeDeviceCPP` refer to `Option 1. Process videos on the EdgeDevice`.
3. Upload `sourcecamera.json` into `/data/local/tmp`.
    * `adb push <host sourcecamera.json path> /data/local/tmp/sourcecamera.json`
4. (Only for `Option 2. Process videos on SourceCameras`) Upload videos specified in `sourcecamera.json`.
    * `adb push <host video path> <adb video path>`
5. Run applications with the Android Studio and click the switch button on the screen.

#### Option 3. Process camera streams of SourceCameras
1. Build and install `EdgeDeviceCPP`, `SoureCamera`, and `EdgeHub` to different mobile devices.
2. Setup `EdgeDeviceCPP` refer to `Option 1. Process videos on the EdgeDevice`.
3. Upload `sourcecamera.json` into `/data/local/tmp`.
    * `adb push <host sourcecamera.json path> /data/local/tmp/sourcecamera.json`
4. Run applications with the Android Studio and click the switch button on the screen.


## Getting Started (Java)

### Prerequisites
1. Download models to use from google drive.
2. (Optional, For TensorFlow Lite models) Put TensorFlow Lite models into `EdgeDevice/src/main/asserts/` directory.
3. (Optional) Download source videos from google drive.

### Installation
1. Download the Android Studio.
2. Open this project with the Android Studio.
3. Select each application to install. (`EdgeDevice`, `SourceCamera`, `EdgeHub`)
4. Build the target application.

### How to Run
* For all options, do same as C++ without `cpp` in config names.


## Contact
Doil Yoon - doil.yoon@hcs.snu.ac.kr
Changmin Jeon - wisechang1@snu.ac.kr

Project Link: [https://github.com/snuhcs/offloading-system](https://github.com/snuhcs/offloading-system)


## Citation
If you find our work useful, please cite our paper below!
```
Will be added
```
