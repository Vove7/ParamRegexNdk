package cn.vove7.paramregex

/**
 * Created by 11324 on 2019/8/15
 */
class ParamRegex(regex: String) {

    private val nativeContext: Long

    init {
        nativeContext = init(regex)
    }

    private external fun init(regex: String): Long

    fun match(text: String): Map<String, String>? {
        return match(nativeContext, text)
    }

    fun finalize() {
        destroy(nativeContext)
    }

    //释放jni对象
    private external fun destroy(cp: Long)

    private external fun match(cp: Long, text: String): Map<String, String>

    companion object {
        init {
            System.loadLibrary("pr")
        }
    }

}

fun String.toParamRegex(): ParamRegex = ParamRegex(this)