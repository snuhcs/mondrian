cmake_minimum_required(VERSION 3.18.1)

project(strm)

set(CMAKE_CXX_STANDARD 17)

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
        strm
        SHARED
        strm/src/Config.cpp
        strm/src/DataType.cpp
        strm/src/DecisionTree.cpp
        strm/src/Interpolator.cpp
        strm/src/Logger.cpp
        strm/src/PatchMixer.cpp
        strm/src/PatchReconstructor.cpp
        strm/src/RoIResizer.cpp
        strm/src/RoIExtractor.cpp
        strm/src/SpatioTemporalRoIMixer.cpp
        strm/src/Test.cpp
        strm/src/Time.cpp
        strm/src/Utils.cpp
        strm/src/impl/CustomInferenceEngine.cpp
        strm/src/impl/ImplConfig.cpp
        strm/src/impl/Worker.cpp
        strm/src/impl/models/Classifier.cpp
        strm/src/impl/models/MnnYoloV4Classifier.cpp
        strm/src/impl/models/TfLiteYoloV4Classifier.cpp
        strm/src/impl/models/TfLiteYoloV5Classifier.cpp
        jni/SpatioTemporalRoIMixer_jni.cpp
)

target_link_libraries(
        strm
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
        MNN
        MNN_CL
        MNN_Express
)
