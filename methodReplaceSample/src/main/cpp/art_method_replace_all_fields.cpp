//
// Created by mawenqiang on 2020/10/9.
//
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include <stdbool.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <jni.h>
#include "common.h"
#include "art_7_0.h"


extern void replace_Method_Field_7_0(JNIEnv* env, jclass clazz, jobject src, jobject dest){
    art::mirror::ArtMethod* smeth =
            (art::mirror::ArtMethod*) env->FromReflectedMethod(src);

    art::mirror::ArtMethod* dmeth =
            (art::mirror::ArtMethod*) env->FromReflectedMethod(dest);
//    reinterpret_cast<art::mirror::Class*>(dmeth->declaring_class_)->clinit_thread_id_ =
//            reinterpret_cast<art::mirror::Class*>(smeth->declaring_class_)->clinit_thread_id_;
//    reinterpret_cast<art::mirror::Class*>(dmeth->declaring_class_)->status_ =
//            static_cast<art::mirror::Class::Status>(
//                    reinterpret_cast<art::mirror::Class *>(smeth->declaring_class_)->status_ - 1);
//    //for reflection invoke
//    reinterpret_cast<art::mirror::Class*>(dmeth->declaring_class_)->super_class_ = 0;

    smeth->declaring_class_ = dmeth->declaring_class_;
    smeth->access_flags_ = dmeth->access_flags_  | 0x0001;
    smeth->dex_code_item_offset_ = dmeth->dex_code_item_offset_;
    smeth->dex_method_index_ = dmeth->dex_method_index_;
    smeth->method_index_ = dmeth->method_index_;
    smeth->hotness_count_ = dmeth->hotness_count_;

    smeth->ptr_sized_fields_.dex_cache_resolved_methods_ =
            dmeth->ptr_sized_fields_.dex_cache_resolved_methods_;
    smeth->ptr_sized_fields_.dex_cache_resolved_types_ =
            dmeth->ptr_sized_fields_.dex_cache_resolved_types_;

    smeth->ptr_sized_fields_.entry_point_from_jni_ =
            dmeth->ptr_sized_fields_.entry_point_from_jni_;
    smeth->ptr_sized_fields_.entry_point_from_quick_compiled_code_ =
            dmeth->ptr_sized_fields_.entry_point_from_quick_compiled_code_;

    LOGW("replace_Method_Field_7_0: %d , %d",
         smeth->ptr_sized_fields_.entry_point_from_quick_compiled_code_,
         dmeth->ptr_sized_fields_.entry_point_from_quick_compiled_code_);
}
