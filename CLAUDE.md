# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mondrian is a multi-stream video analytics system that performs spatio-temporal packing of regions of interest (ROIs) for efficient object detection on Android devices. It combines an Android runtime (Java + C++ via JNI) with a Python offline training stage.

## Build & Run

### Android Runtime (`android/`)

**IDE**: Android Studio  
**Build**: Open `android/` in Android Studio, then build the `Mondrian` module.  
**Command-line build**: `cd android && ./gradlew assembleDebug`

- Compile/Target SDK: 30 (Android 12+)
- ABI: arm64-v8a only
- C++ Standard: C++17, built via CMake 3.18.1
- Java: version 1.9

**Prerequisites**: Download native dependencies (jniLibs, third_party libs, TFLite models) from the project's Google Drive before building.

**Deploy to device**:
```bash
adb push android/config.json /data/local/tmp/config.json
adb push <model_path> /data/local/tmp/models/
adb push <video_path> <device_video_path>
```

**Helper scripts** (in `android/`):
- `update_config.sh` — push config.json to device
- `pull_logs.sh` — pull log files from device
- `get_csvs.sh` — pull CSV result files

### Offline Stage (`offline/`)

**Python dependencies**: `pip install -r offline/requirements.txt` (scikit-learn, OpenCV, numpy, pandas, scipy, matplotlib, tqdm)

Key scripts:
- `generate_packed_frames.py` — generate packed video frames for evaluation
- `train_roi_scale_estimator.py` — train ROI scale estimator (outputs C++ decision tree code for `tree/MTA.cpp` and `tree/VIRAT.cpp`)

### Tests

```bash
cd android && ./gradlew test              # JUnit unit tests
cd android && ./gradlew connectedAndroidTest  # Instrumented tests (requires device)
```

## Architecture

### Threading Model (7 threads)

The runtime pipeline is a multi-threaded producer-consumer chain:

1. **VideoLoader** — decodes video frames (YUV format)
2. **ROIExtractor::PDThread** — person detection to extract ROIs
3. **ROIExtractor::OFThread** — optical flow tracking + PD filtering
4. **ROIExtractor::PostprocessThread** — ROI scaling and merging
5. **Mondrian::scheduleThread** — inference scheduling + ROI packing
6. **Mondrian::postprocessThread** — bounding box reconstruction + interpolation
7. **Mondrian::logThread** — writes results to log files

Each thread passes data via named queues (e.g., `PDWaiting_`, `OFWaiting_`, `PostprocessWaiting_`, `Processed_`, `packingResults_`, `results_`).

### Code Layers

**Java layer** (`android/Mondrian/src/main/java/hcs/offloading/mondrian/`): Android UI, video surface management, JNI bridge (`MondrianApp` manages native handle lifecycle).

**JNI bridge** (`cpp/jni/Mondrian_jni.cpp`): Connects Java calls to the C++ engine.

**C++ engine** (`cpp/mondrian/`): Core processing pipeline.
- `Mondrian.cpp` — main orchestrator, owns the thread pipeline
- `ROIExtractor` / `ROIPacker` / `ROIResizer` / `ROIPrioritizer` — ROI processing stages
- `InferenceEngine` / `InferencePlanner` — manages TFLite inference (GPU/NNAPI delegates)
- `model/TfLiteYoloV5Classifier*` — YOLOv5 model wrappers (standard + DSP variants)
- `PatchReconstructor` / `Interpolator` — post-inference box reconstruction
- `PackedCanvas` — canvas-based spatial packing of ROIs
- `tree/MTA.cpp`, `tree/VIRAT.cpp` — auto-generated decision trees from offline training

### Native Dependencies

Linked via CMake (`cpp/CMakeLists.txt`): OpenCV (core, dnn, video, videoio, imgcodecs, imgproc), TensorFlow Lite (with GPU delegate), NNAPI, jsoncpp.

### Configuration

`android/config.json` controls all runtime behavior: video sources, ROI extraction parameters, inference engine settings (model, device, frame size), packing strategy, and logging flags.
