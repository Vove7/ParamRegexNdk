#ifndef STR_UTILS
#define STR_UTILS

#include "jni.h"

#include<cstdlib>
#include<string>
#include<cstring>

std::string ws2s(const std::wstring &ws) {
    std::string strLocale = setlocale(LC_ALL, "");
    const wchar_t *wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char *chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs(chDest, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[] chDest;
    setlocale(LC_ALL, strLocale.c_str());
    return strResult;
}

// string => wstring
std::wstring s2ws(const std::string &s) {
    std::string strLocale = setlocale(LC_ALL, "");
    const char *chSrc = s.c_str();
    size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
    wchar_t *wchDest = new wchar_t[nDestSize];
    wmemset(wchDest, 0, nDestSize);
    mbstowcs(wchDest, chSrc, nDestSize);
    std::wstring wstrResult = wchDest;
    delete[] wchDest;
    setlocale(LC_ALL, strLocale.c_str());
    return wstrResult;
}


std::string js2s(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("UTF-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

jstring s2js(JNIEnv *env, std::string jstr) {
    return env->NewStringUTF(jstr.c_str());
}

std::string i2s(int a) {
    char *s = new char[5];
    sprintf(s, "%d", a);
    return s;
}

std::string l2s(long a) {
    char *s = new char[10];
    sprintf(s, "%li", a);
    return s;
}

#endif // !STR_UTILS

