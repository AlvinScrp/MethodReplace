package com.a.ndksample;

import android.util.Log;

import com.fix.M;
import com.fix.MFix;
import com.fix.MethodSize;

import java.lang.reflect.Method;

public class NDKTools {
    static {
        System.loadLibrary("hello");
    }

    public static native String getHelloContent();

    public static native String sayHello();

    public static native void  replaceMethod(Method mthA, Method mthB);


    public static void doReplace() {
        try{
            new MFix();
            Method mthA=M.class.getDeclaredMethod("a");
            Method mthB= MFix.class.getDeclaredMethod("a");
            NDKTools.replaceMethod(mthA,mthB);
        }catch (Exception e){
            e.printStackTrace();
        }

    }

}
