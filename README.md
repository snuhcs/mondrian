<div id="top"></div>

# Multi-Camera Spatio-temporal RoI Mixing
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
1. Download MNN and TensorFlow Lite models from google drive.
2. Put downloaded models in `EdgeServer/src/main/assets` directory.

### Installation
1. Download the Android Studio.
2. Open this project with the Android Studio.
3. Select each application to install. (`edgeserver`, `sensorapplication`, `scheduler`)
4. Build the target application.

### How to Run
1. Install applications to different mobile devices with the Android Studio.
2. Upload `edgeserver.json` into `/data/local/tmp`. (`adb push edgeserver.json /data/local/tmp`)
3. Run applications with the Android Studio.


## Contact
Doil Yoon - doil.yoon@hcs.snu.ac.kr
Changmin Jeon - wisechang1@snu.ac.kr

Project Link: [https://github.com/snuhcs/offloading-system](https://github.com/snuhcs/offloading-system)


## Citation
If you find our work useful, please cite our paper below!
```
Will be added
```
