package cn.vove7.ndkdemo

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.widget.SeekBar
import cn.vove7.paramregex.ParamRegex
import cn.vove7.paramregex.toParamRegex
import kotlinx.android.synthetic.main.activity_main.*
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    var testCount = 1000
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                result_text.text = progress.toString()
                testCount = progress
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })
    }

    fun doMatch(v: View) {
        val begin = System.currentTimeMillis()
        try {
            val map = ParamRegex(reg_text.text.toString())
                    .match(match_text.text.toString())
            if (map == null) {
                result_text.text = "匹配失败"
            } else {
                result_text.text = map.toString()
            }
        } catch (e: Throwable) {
            result_text.text = e.message
            e.printStackTrace()
        }
        val end = System.currentTimeMillis()
        result_text.append("\n耗时：${end - begin}ms")
    }
    //ndk 同一实例
    fun batchMatch1(v: View) {
        thread {
            val begin = System.currentTimeMillis()
            val reg = "[0-9]*(你)?好%啦[123]+".toParamRegex()
            for (i in 0..testCount) {
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch1耗时：${end - begin}ms")
            }
        }
    }

    //ndk 新建实例
    fun batchMatch2(v: View) {
        thread {
            val begin = System.currentTimeMillis()
            for (i in 0..testCount) {
                val reg = "[0-9]*(你)?好%啦[123]+".toParamRegex()
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch1耗时：${end - begin}ms")
            }
        }
    }

    //kotlin 同一实例
    fun batchMatch3(v: View) {
        thread {
            val begin = System.currentTimeMillis()
            val reg = cn.vove7.regEngine.ParamRegex("[0-9]*(你)?好%啦[123]+")
            for (i in 0..testCount) {
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch3耗时：${end - begin}ms")
            }
        }
    }

    //kotlin 新建实例
    fun batchMatch4(v: View) {
        thread {
            val begin = System.currentTimeMillis()
            for (i in 0..testCount) {
                val reg = cn.vove7.regEngine.ParamRegex("[0-9]*(你)?好%啦[123]+")
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch4耗时：${end - begin}ms")
            }
        }
    }

}
