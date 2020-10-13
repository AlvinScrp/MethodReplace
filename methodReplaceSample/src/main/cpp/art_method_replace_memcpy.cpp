//
// Created by mawenqiang on 2020/10/9.
//

#include <jni.h>
#include <cstring>
#include "common.h"


static int methodSize;

extern  int calculateMethodSize(JNIEnv *env) {
    jclass clz=env->FindClass("com/fix/MethodSize");
    jbyte *f1 = reinterpret_cast<jbyte *>(env->GetStaticMethodID(clz, "f1", "()V"));
    jbyte *f2 = reinterpret_cast<jbyte *>(env->GetStaticMethodID(clz, "f2", "()V"));
    methodSize = (jbyte *) f2 - (jbyte *) f1;
    return methodSize;

}

extern void replace_memcpy(JNIEnv *env, jclass clazz, jobject src, jobject dest) {

    LOGD("replace memcpy methodSize %d",methodSize);
    void *smethodIds = env->FromReflectedMethod(src);
    void *dmethodIds = env->FromReflectedMethod(dest);
    memcpy(smethodIds, dmethodIds, methodSize);

}
