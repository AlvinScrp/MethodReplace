//
// Created by mawenqiang on 2020/9/25.
//

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

/*
 * Class:     com_a_ndksample_NDKTools
 * Method:    getHelloContent
 * Signature: ()Ljava/lang/String;
 */
extern void replace_Method_Field_7_0(JNIEnv *env, jclass clazz, jobject src, jobject dest);

extern void replace_memcpy(JNIEnv *env, jclass clazz, jobject src, jobject dest);

extern int calculateMethodSize(JNIEnv *env);

static jstring getHelloContent
        (JNIEnv *env, jclass jclass1) {
    return env->NewStringUTF("hello Ndk2222333");
}

static jstring sayHello(JNIEnv *env, jclass jclass1) {
    LOGD("native: say hello ###");
    return env->NewStringUTF("say Hello");
}

static void replaceMethod(JNIEnv *env, jclass clazz, jobject src, jobject dest) {
    replace_Method_Field_7_0(env,clazz,src, dest);
//    replace_memcpy(env, clazz, src, dest);
}


static const char *className = "com/a/ndksample/NDKTools";



static JNINativeMethod gJni_Methods_table[] = {
        {"getHelloContent", "()Ljava/lang/String;", (jstring *) getHelloContent},
        {"sayHello",        "()Ljava/lang/String;", (jstring *) sayHello},
        {"replaceMethod",
                            "(Ljava/lang/reflect/Method;Ljava/lang/reflect/Method;)V",
                                                    (void *) replaceMethod},
};

static int jniRegisterNativeMethods(JNIEnv *env, const char *className,
                                    const JNINativeMethod *gMethods) {
    jclass clazz = (env)->FindClass(className);
    if (clazz == NULL) return -1;
    int numMethods = sizeof(gJni_Methods_table) / sizeof(JNINativeMethod);
    LOGD("nuMethods %d", numMethods);
    int result = (env)->RegisterNatives(clazz, gJni_Methods_table, numMethods);
    LOGD("RegisterNatives '%s' , result: %d\n", className, result);
    (env)->DeleteLocalRef(clazz);
    return result;
}



jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("enter jni_onload");

    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    jniRegisterNativeMethods(env, className, gJni_Methods_table);
    calculateMethodSize(env);

    return JNI_VERSION_1_4;
}

