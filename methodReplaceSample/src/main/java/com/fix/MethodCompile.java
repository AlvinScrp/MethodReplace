package com.fix;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

public class MethodCompile {

    public static void test1(Context context){
        Toast.makeText(context.getApplicationContext(),"old apk",Toast.LENGTH_SHORT).show();
        print("test1");
    }

    private static void print(String s) {
        Log.d("alvin",s);
    }

    public static void test2(Context context){
        Log.d("alvin","test2");
    }

    public static void test3(Context context){
        Log.d("alvin","test3");
    }

}
