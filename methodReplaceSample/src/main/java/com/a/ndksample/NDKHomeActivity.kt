package com.a.ndksample

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.fix.M
import com.fix.MFix
import com.fix.MethodCompile
import kotlinx.android.synthetic.main.activity_ndk_home.*
import java.lang.Exception

class NDKHomeActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ndk_home)

        tv.text = M.a()
        btnRepalce.setOnClickListener {
            NDKTools.doReplace()
            tv.text = M.a()
        }
    }
}