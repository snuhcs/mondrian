<div id="top"></div>

# Multi-Camera Spatio-temporal RoI Mixing
FYC, [STRM/src/main/cpp/strm](https://github.com/snuhcs/offloading-system/tree/master/STRM/src/main/cpp/strm)
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


## Getting Started

### Prerequisites
1. Download dependencies into `STRM/src/main/jniLibs` and `STRM/src/main/cpp/third_party` from google drive.
2. Download models to use from google drive.
3. (Optional) Download source videos from google drive.

### Installation
1. Download the Android Studio.
2. Open this project with the Android Studio.
3. Build and install the `STRM` application.

### How to Run
1. Build and install `STRM` application.
2. Upload `strm.json` into `/data/local/tmp`.
    * `adb push <host strm.json path> /data/local/tmp/strm.json`
3. Upload models to use into `/data/local/tmp/models/`.
    * `adb push <host model path> /data/local/tmp/models`
4. Upload videos specified in `strm.json`.
    * `adb push <host video path> <adb video path>`
5. Run `STRM`.


## Contact
Changmin Jeon - wisechang1@snu.ac.kr

Doil Yoon - doil.yoon@hcs.snu.ac.kr

Project Link: [https://github.com/snuhcs/offloading-system](https://github.com/snuhcs/offloading-system)


## Citation
If you find our work useful, please cite our paper below!
```
Will be added
```
