#include <jni.h>

#include "SpatioTemporalRoIMixer.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_createSpatioTemporalRoIMixer(JNIEnv *env,
                                                                                jobject thiz) {
    // TODO: implement createSpatioTemporalRoIMixer()
    return reinterpret_cast<jlong>(new SpatioTemporalRoIMixer());
}
extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_enqueueImage(JNIEnv *env, jobject thiz,
                                                                jlong strm_handle, jstring key,
                                                                jint frame_index, jobject mat) {
    // TODO: implement enqueueImage()
    auto* strm = (SpatioTemporalRoIMixer*) strm_handle;
    strm->enqueueImage();
}