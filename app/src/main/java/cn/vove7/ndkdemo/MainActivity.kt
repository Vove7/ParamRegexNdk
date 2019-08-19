package cn.vove7.ndkdemo

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
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

        val examples = listOf(
                "(打开|显示|下拉)?通知栏" to "通知栏",
                "(打开|显示|下拉)通知栏" to "通知栏",
                "[0-9]*(你)?好@{a}?啦@{#num1}分@{#num2}" to "131好123啦38分二十五",
                "(扫描|识别)(屏幕(上的)?)?(二维码|条形?码)" to "识别屏幕二维码",
                "(#楼)?过道" to "1楼过道",
                "你好?啊" to "你好啊",
                "(帮我)?(打开|关闭|关掉)?(客厅|(#楼)?过道)?的?(空调|电灯)" to "打开5楼过道电灯",
                "(帮我)?(打开|关闭|关掉)?(客厅|茶室|餐厅|厨房|主卧|卧室|次卧|衣帽间|书房|影视厅|一楼|二楼|三楼|车库|门厅|办公室|地下室|((#楼)?楼梯间)|((#楼)?过道)|过道|楼梯间|酒窖|二楼楼梯间|试验房间)?的?(灯|电灯|筒灯|大灯|吊灯|灯带|布帘|纱帘|窗帘|楼梯灯|楼道灯|门锁)%" to "打开客厅大灯"

        )

        spinner.adapter = ArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, examples.map { it.first })
        spinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onNothingSelected(parent: AdapterView<*>?) {
            }

            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                reg_text.setText(examples[position].first)
                match_text.setText(examples[position].second)
            }
        }

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

    private val ndkTestRegex: ParamRegex get() = ParamRegex(reg_text.text.toString())
    private val ktTestRegex: cn.vove7.regEngine.ParamRegex get() = cn.vove7.regEngine.ParamRegex(reg_text.text.toString())

    fun doMatch(v: View) {
        val begin = System.currentTimeMillis()
        try {
            val map = ndkTestRegex.match(match_text.text.toString())
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
                val reg = ndkTestRegex
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch2耗时：${end - begin}ms")
            }
        }
    }

    //kotlin 同一实例
    fun batchMatch3(v: View) {
        thread {
            val begin = System.currentTimeMillis()
            val reg = ktTestRegex
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
                val reg = ktTestRegex
                reg.match("131好123啦22312")
            }
            val end = System.currentTimeMillis()
            runOnUiThread {
                result_text.text = ("batchMatch4耗时：${end - begin}ms")
            }
        }
    }

}
