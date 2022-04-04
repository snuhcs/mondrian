
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project(strmcpp)

include_directories(include)
link_directories(lib)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(
        strmcpp         # Sets the name of the library.
        SHARED          # Sets the library as a shared library.
        src/Config.cpp  # Provides a relative path to your source file(s).
        src/Dispatcher.cpp
        src/PatchMixer.cpp
        src/PatchReconstructor.cpp
        src/RoIExtractor.cpp
        src/SpatioTemporalRoIMixer.cpp
        src/CppInferenceEngine.cpp
        src/YoloV4Classifier.cpp
        jni/SpatioTemporalRoIMixer_jni.cpp
)

add_library(
        jsoncpp
        SHARED
        ${CMAKE_SOURCE_DIR}/lib/jsoncpp.cpp
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(
        log-lib  # Sets the name of the path variable.
        log  # Specifies the name of the NDK library that you want CMake to locate.
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        strmcpp     # Specifies the target library.
        ${log-lib}  # Links the target library to the log library included in the NDK.
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
