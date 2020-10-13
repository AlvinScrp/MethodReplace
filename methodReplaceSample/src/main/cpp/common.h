//
// Created by mawenqiang on 2020/10/9.
//

#ifndef ANDFIXDEMO_COMMON_H
#define ANDFIXDEMO_COMMON_H

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "JNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif //ANDFIXDEMO_COMMON_H
