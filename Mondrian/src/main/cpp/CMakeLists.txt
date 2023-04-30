cmake_minimum_required(VERSION 3.18.1)

project(mondrian)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    mondrian/include/
    third_party/include/
)

link_directories(
    third_party/lib
)

add_library(
    jsoncpp
    SHARED
    third_party/lib/jsoncpp.cpp
)

find_library(
    log-lib
    log
)

add_library(
    mondrian
    SHARED
    mondrian/src/Config.cpp
    mondrian/src/DataType.cpp
    mondrian/src/Frame.cpp
    mondrian/src/FrameBuffer.cpp
    mondrian/src/InferenceEngine.cpp
    mondrian/src/InferencePlanner.cpp
    mondrian/src/Interpolator.cpp
    mondrian/src/Logger.cpp
    mondrian/src/MixedFrame.cpp
    mondrian/src/Mondrian.cpp
    mondrian/src/PatchMixer.cpp
    mondrian/src/PatchReconstructor.cpp
    mondrian/src/ROI.cpp
    mondrian/src/ROIExtractor.cpp
    mondrian/src/ROIResizer.cpp
    mondrian/src/Test.cpp
    mondrian/src/Time.cpp
    mondrian/src/Utils.cpp
    mondrian/src/Worker.cpp
    mondrian/src/tree/MTA.cpp
    mondrian/src/tree/VIRAT.cpp
    mondrian/src/model/Classifier.cpp
    mondrian/src/model/MnnYoloV4Classifier.cpp
    mondrian/src/model/TfLiteYoloV4Classifier.cpp
    mondrian/src/model/TfLiteYoloV5Classifier.cpp
    mondrian/src/model/TfLiteYoloV5ClassifierDSP.cpp
    jni/Mondrian_jni.cpp
)

target_link_libraries(
    mondrian
    ${log-lib}
    jsoncpp
    opencv_core
    opencv_dnn
    opencv_video
    opencv_videoio
    opencv_imgcodecs
    opencv_imgproc
    tensorflowlite
    tensorflowlite_gpu_delegate
    nnapi_delegate_no_nnapi_implementation
    nnapi_implementation
    nnapi_util
    MNN
    MNN_CL
    MNN_Express
)
