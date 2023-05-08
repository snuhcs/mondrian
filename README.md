<div id="top"></div>

# Mondrian
FYC, [Mondrian/src/main/cpp/mondrian](https://github.com/snuhcs/mondrian/tree/master/Mondrian/src/main/cpp/mondrian)
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
* [Android Studio](https://developer.android.com/studio/)
* [TensorFlow Lite](https://www.tensorflow.org/lite/)
* [MNN](https://github.com/alibaba/MNN/)


## Getting Started

### Prerequisites
1. Download dependencies into `android/Mondrian/src/main/jniLibs` and `android/Mondrian/src/main/cpp/third_party` from google drive.
2. Download models to use from google drive.
3. (Optional) Download source videos from google drive.

### Installation
1. Download the Android Studio.
2. Open `android/` directory with the Android Studio.
3. Build and install the `Mondrian` application.

### How to Run
1. Build and install `Mondrian` application.
2. Upload `android/config.json` into `/data/local/tmp`.
    * `adb push <host config.json path> /data/local/tmp/config.json`
3. Upload models to use into `/data/local/tmp/models/`.
    * `adb push <host model path> /data/local/tmp/models`
4. Upload videos specified in `config.json`.
    * `adb push <host video path> <adb video path>`
5. Run `Mondrian`.


## Contact
* Changmin Jeon - wisechang1@snu.ac.kr
* Seonjun Kim - seonjun.kim@hcs.snu.ac.kr
* Project Link: [https://github.com/snuhcs/mondrian](https://github.com/snuhcs/mondrian)


## Citation
If you find our work useful, please cite our paper below!
```
Will be added
```
