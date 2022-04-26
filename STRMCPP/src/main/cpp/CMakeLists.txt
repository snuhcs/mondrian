cmake_minimum_required(VERSION 3.18.1)

project(strmcpp)

include_directories(
        third_party/include/
        strm/include/
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
        strmcpp
        SHARED
        strm/src/Config.cpp
        strm/src/Time.cpp
        strm/src/Dispatcher.cpp
        strm/src/PatchMixer.cpp
        strm/src/PatchReconstructor.cpp
        strm/src/RoIExtractor.cpp
        strm/src/SpatioTemporalRoIMixer.cpp
        strm/src/impl/ImplConfig.cpp
        strm/src/impl/CustomInferenceEngine.cpp
        strm/src/impl/Worker.cpp
        strm/src/impl/YoloV4Classifier.cpp
        jni/SpatioTemporalRoIMixer_jni.cpp
)

target_link_libraries(
        strmcpp
        ${log-lib}
        jsoncpp
        opencv_core
        opencv_dnn
        opencv_video
        opencv_imgcodecs
        opencv_imgproc
        MNN
        MNN_CL
        MNN_Express
)
