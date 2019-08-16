#include <jni.h>
#include "cn_vove7_paramregex_ParamRegex.h"
#include"ParamRegex.cpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_cn_vove7_paramregex_ParamRegex_init
        (JNIEnv *env, jobject obj, jstring regex) {
    ParamRegex *pr = new ParamRegex(js2s(env, regex));
    return (jlong) pr;
}


JNIEXPORT jobject JNICALL Java_cn_vove7_paramregex_ParamRegex_match
        (JNIEnv *env, jobject obj, jlong cp, jstring text) {
    auto *pr = (ParamRegex *) cp;
    try {
        map<string, string> *result = pr->match(js2s(env, text));
        if (result != nullptr)
            return ToHashMap(env, result);
        else return nullptr;
    }
    catch (const char* e) {//抛出java 异常
        jclass clazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(clazz, e);
    }
    catch (const string& e) {//抛出java 异常
        jclass clazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(clazz, e.c_str());
    }
    catch (...){
        jclass clazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(clazz, "未知错误");
    }
    return nullptr;
}

JNIEXPORT void JNICALL Java_cn_vove7_paramregex_ParamRegex_destroy
        (JNIEnv *env, jobject obj, jlong cp) {
    auto *pr = (ParamRegex *) cp;
    delete pr;
}


}