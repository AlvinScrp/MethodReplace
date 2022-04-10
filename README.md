# 概述
方法热替换是一种运行时方法Hook技术的应用，达到调用旧方法实际执行新方法的目的。
在Dalvik/ART中，方法是一个Method对象存放在对应的内存区域中，通过method_idx映射到具体对象。
我们替换的是Method对象的属性。
![截屏2020-10-13上午11.29.56.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602559862740-54d4b5cd-af51-4d17-a51f-420236d50fc8.png#align=left&display=inline&height=386&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-13%E4%B8%8A%E5%8D%8811.29.56.png&originHeight=386&originWidth=1238&size=41608&status=done&style=none&width=1238)
# 准备知识
## Android 代码是怎么执行的
在 Android 中，Java 类被转换成 DEX 字节码。DEX 字节码通过 ART 或者 Dalvik runtime 转换成机器码。这里 DEX 字节码和设备架构无关。
Dalvik 是一个基于 JIT（Just in time）编译的引擎。使用 Dalvik 存在一些缺点，所以从 Android 4.4（Kitkat）开始引入了 ART 作为运行时，从 Android 5.0（Lollipop）开始 ART 就全面取代了Dalvik。Android 7.0 向 ART 中添加了一个 just-in-time（JIT）编译器，这样就可以在应用运行时持续的提高其性能。
**重点：**Dalvik 使用 JIT（Just in time）编译而 ART 使用 AOT（Ahead of time）编译。
### JVM VS Dalvik
下图描述了 Dalvik 虚拟机和 Java 虚拟机之间的差别。
![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602560518592-d34e5f19-6866-4489-a299-b4172701942d.png#align=left&display=inline&height=889&margin=%5Bobject%20Object%5D&name=image.png&originHeight=889&originWidth=964&size=201434&status=done&style=none&width=964)
### Dalvik VS ART
![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602560549593-e1aead19-e338-412a-b41e-ccd33e3c422d.png#align=left&display=inline&height=916&margin=%5Bobject%20Object%5D&name=image.png&originHeight=916&originWidth=1040&size=216333&status=done&style=none&width=1040)
## Class类文件结构
![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602595695072-402af941-4b85-44ac-b50c-d18a16e90831.png#align=left&display=inline&height=268&margin=%5Bobject%20Object%5D&name=image.png&originHeight=536&originWidth=435&size=34597&status=done&style=none&width=217.5)
![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602595705218-cf4d0a78-3880-4472-a386-3afcea9cfbe8.png#align=left&display=inline&height=271&margin=%5Bobject%20Object%5D&name=image.png&originHeight=541&originWidth=802&size=110971&status=done&style=none&width=401)
Class文件结构采用类似C语言的结构体来存储数据的，主要有两类数据项，无符号数和表，无符号数用来表述数字，索引引用以及字符串等，比如 u1，u2，u4，u8分别代表1个字节，2个字节，4个字节，8个字节的无符号数，而表是有多个无符号数以及其它的表组成的复合结构，习惯地以_info结尾。表用于描述有层次关系的符合结构的数据，整个Class文件本质上就是一张表。
```
ClassFile {
    u4             magic;
    u2             minor_version;
    u2             major_version;
    u2             constant_pool_count;
    cp_info        constant_pool[constant_pool_count-1];
    u2             access_flags;
    u2             this_class;
    u2             super_class;
    u2             interfaces_count;
    u2             interfaces[interfaces_count];
    u2             fields_count;
    field_info     fields[fields_count];
    u2             methods_count;
    method_info    methods[methods_count];
    u2             attributes_count;
    attribute_info attributes[attributes_count];
}
```
## Dex文件
Android 平台中没有直接使用 Class 文件格式，因为早期的 Anrdroid 手机内存，存储都比较小，而 Class 文件显然有很多可以优化的地方，比如每个 Class 文件都有一个常量池，里边存储了一些字符串。一串内容完全相同的字符串很有可能在不同的 Class 文件的常量池中存在，这就是一个可以优化的地方。当然，Dex 文件结构和 Class 文件结构差异的地方还很多，但是从携带的信息上来看，Dex 和 Class 文件是一致的。所以，你了解了 Class 文件（作为 Java VM 官方 Spec 的标准），Dex 文件结构只不过是一个变种罢了（从学习到什么程度为止的问题来看，如果不是要自己来解析 Dex 文件，或者反编译 / 修改 dex 文件，我觉得大致了解下 Dex 文件结构的情况就可以了）。
###  Dex 文件结构
![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602566473306-ad9c6e97-4f5b-4db6-b6c6-28f91c9578db.png#align=left&display=inline&height=361&margin=%5Bobject%20Object%5D&name=image.png&originHeight=361&originWidth=600&size=111732&status=done&style=none&width=600)

### ArtMethod结构体
Art运行时，会把Class加载到内存中，从dex中读取方法，以ArtMethod对象数组的形式存放在内存中。
ArtMethod7.0地址在art/runtime/art_method.h中。不同版本的结构是有差异的
```cpp
class ArtMethod {
            uint32_t declaring_class_;
            uint32_t access_flags_;
            uint32_t dex_code_item_offset_;
            uint32_t dex_method_index_;
            uint16_t method_index_;uint16_t hotness_count_;
            struct PtrSizedFields {
                ArtMethod **dex_cache_resolved_methods_;
                void *dex_cache_resolved_types_;
                void *entry_point_from_jni_;
                void *entry_point_from_quick_compiled_code_;
            } ptr_sized_fields_;
};
```
### ArtMethodArray
art/runtime/class_linker.cc
```cpp
void ClassLinker::LoadClassMembers(Thread* self,
                                   const DexFile& dex_file,
                                   const uint8_t* class_data,
                                   Handle<mirror::Class> klass,
                                   const OatFile::OatClass* oat_class) {
    ...
    // Load methods.
    klass->SetMethodsPtr(
        AllocArtMethodArray(self, allocator, it.NumDirectMethods() + it.NumVirtualMethods()),
        it.NumDirectMethods(),
        it.NumVirtualMethods());
    ...
}
LengthPrefixedArray<ArtMethod>* ClassLinker::AllocArtMethodArray(Thread* self,
                                                                 LinearAlloc* allocator,
                                                                 size_t length) {
  ...
  const size_t method_alignment = ArtMethod::Alignment(image_pointer_size_);
  const size_t method_size = ArtMethod::Size(image_pointer_size_);
  const size_t storage_size =
      LengthPrefixedArray<ArtMethod>::ComputeSize(length, method_size, method_alignment);
  void* array_storage = allocator->Alloc(self, storage_size);
  auto* ret = new (array_storage) LengthPrefixedArray<ArtMethod>(length);
  CHECK(ret != nullptr);
  for (size_t i = 0; i < length; ++i) {
    new(reinterpret_cast<void*>(&ret->At(i, method_size, method_alignment))) ArtMethod;
  }
  return ret;
}
```

### Art方法调用原理

在ART中，每一个Java方法在虚拟机（注：ART与虚拟机虽有细微差别，但本文不作区分，两者含义相同，下同）内部都由一个ArtMethod对象表示（native层，实际上是一个C++对象），这个native 的 ArtMethod对象包含了此Java方法的所有信息，比如名字，参数类型，方法本身代码的入口地址（entrypoint)等；暂时放下trampoline以及interpreter和jit不谈，一个Java方法的执行非常简单：

1. 想办法拿到这个Java方法所代表的ArtMethod对象
1. 取出其entrypoint，然后跳转到此处开始执行

![image.png](https://cdn.nlark.com/yuque/0/2020/png/1305846/1602567334253-13991a65-dd92-4901-b606-d961d14c84cc.png#align=left&display=inline&height=859&margin=%5Bobject%20Object%5D&name=image.png&originHeight=859&originWidth=1200&size=489443&status=done&style=none&width=1200)

# 方法替换过程解析
这里我们以Andfix中ArtMethod替换为例来分析，实际的替换是在native层完成的。
[CMake构建NDK项目生成so库](https://www.jianshu.com/p/4c231916af81)
## Java Method
我们定义三个方法，旧方法M.a(),新方法MFix.a(),还有操作JNI的方法NDKTools.replaceMethod(Method,Method)
```java
public class M {
    public static String a() {return "aaa";}
}

public class MFix {
    private static String a() {return "a fixxxx"; }
}

public class NDKTools {
    static {System.loadLibrary("hello"); }
    
    public static native void  replaceMethod(Method mthA, Method mthB);

    public static void doReplace() {
      
       new MFix();
       Method mthA=M.class.getDeclaredMethod("a");
       Method mthB= MFix.class.getDeclaredMethod("a");
       NDKTools.replaceMethod(mthA,mthB);
    }
}
```
## Native ArtMethod指针
JNI提供了FromReflectedMethod方法，转换一个java.lang.reflect.Method对象到一个ArtMethod对象指针
```cpp
//通过JNI RegisterNatives关联 java类中的native void  replaceMethod(..)
static void replaceMethod(JNIEnv *env, jclass clazz, jobject src, jobject dest) {
    art::mirror::ArtMethod* smeth =
            (art::mirror::ArtMethod*) env->FromReflectedMethod(src);
    art::mirror::ArtMethod* dmeth =
            (art::mirror::ArtMethod*) env->FromReflectedMethod(dest);

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
}
```
## 定义ArtMethod
上述代码中的art::mirror::ArtMethod并不是引用 art/runtime/art_method.h中的ArtMethod。其实是把源码抄了出来。还有关联的Object,Class，ArtFields。基于Android7.0。
```cpp
class ArtMethod {
            uint32_t declaring_class_;
            uint32_t access_flags_;
            uint32_t dex_code_item_offset_;
            uint32_t dex_method_index_;
            uint16_t method_index_;uint16_t hotness_count_;
    		struct PtrSizedFields {
            	ArtMethod **dex_cache_resolved_methods_;
            	void *dex_cache_resolved_types_;
				void *entry_point_from_jni_;
				void *entry_point_from_quick_compiled_code_;
            } ptr_sized_fields_;
};
```

## 运行结果
```cpp
Log.d("Fix",M.a());//输出：aaa
NDKTools.doReplace();
Log.d("Fix",M.a());//输出：a fixxxx
```
## memcpy整体替换
### 使用ArtMethod结构体的局限性
知得注意的是，上述代码在Android7.0可以跑起来，但是在Android8.0就不行了。因为Android8.0的结构定义变了。阿里Andfix针对不同版本Art，分别做了适配。 而阿里Sophix提出了memcpy整体替换的方案。
由于ArtMethod是存放在数组中，且单个版本内ArtMethod的内存长度是固定。所以只要取到ArtMethod的size就可以通过JNI提供的 void *memcpy(void *, const void *, size_t) ，把destMethod的内容复制到 srtMethod即可。
### ArtMethod Size
阿里Sophix给到一个计算size的方法，即在一个类中定义两个方法，计算对应ArtMethod地址差值
```java
public class MethodSize {
    final  public static void f1(){}
    final  public static void f2(){}
}
```
```cpp
static int methodSize;
extern  int calculateMethodSize(JNIEnv *env) {
    jclass clz=env->FindClass("com/fix/MethodSize");
    jbyte *f1 = reinterpret_cast<jbyte *>(env->GetStaticMethodID(clz, "f1", "()V"));
    jbyte *f2 = reinterpret_cast<jbyte *>(env->GetStaticMethodID(clz, "f2", "()V"));
    methodSize = (jbyte *) f2 - (jbyte *) f1;
    return methodSize;
}
```
### memcpy
通过代码我们就可以屏蔽ArtMethod结构体的细节，达到整体替换的目的
```cpp
extern void replace_memcpy(JNIEnv *env, jclass clazz, jobject src, jobject dest) {
    LOGD("replace memcpy methodSize %d",methodSize);
    void *smethodIds = env->FromReflectedMethod(src);
    void *dmethodIds = env->FromReflectedMethod(dest);
    memcpy(smethodIds, dmethodIds, methodSize);
}
```
# 局限性
实际在Android8及以下是可以的。Android9和Android10是不行的，还不清楚原因。
# 小结
[Demo地址](https://github.com/AlvinScrp/MethodReplace)
Demo中包含Android_hotfix.pdf,即阿里图书《深入探索Android热修复技术原理》

参考：
[走进Android运行时 DVM vs ART](https://www.yuque.com/alvin-8c8gz/mpb4rh/okw2qa)
[Dalvik虚拟机介绍](https://www.yuque.com/alvin-8c8gz/mpb4rh/loq8cd)
[豆瓣：深入理解Android：Java虚拟机ART 邓凡平](https://read.douban.com/reader/ebook/110275324/?from=book)
[github alibaba Andfix](https://github.com/alibaba/AndFix)

[ 理解Android虚拟机体系结构](https://www.cnblogs.com/lao-liang/p/5111399.html)

