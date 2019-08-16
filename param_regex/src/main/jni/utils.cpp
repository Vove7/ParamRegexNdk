#ifndef __UTILS__
#define __UTILS__

#include<vector>
#include<string>
#include<sstream>
#include<map>
#include<stack>
#include "strutils.h"
#include "android/log.h"

using namespace std;
//#define LOG_TAG  "PARAM_REGEX"

#ifdef LOG_TAG
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

void split(const string &s, vector<string> &sv, const char flag) {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
}

int str_to_int(const char *str) {
    int num = 0;
    if (str != NULL && *str != '\0') {
        bool minus = false;
        if (*str == '+')
            str++;
        else if (*str == '-') {
            str++;
            minus = true;
        }
        if (*str != '\0')
            while (*str != '\0') {
                if (*str >= '0' && *str <= '9') {
                    int flag = minus ? -1 : 1;
                    num = num * 10 + flag * (*str - '0');
                    if ((!minus && num > 0x7FFFFFFF) || (minus && num < (signed int) 0x80000000)) {
                        num = 0;
                        break;
                    }
                    str++;
                } else {
                    num = 0;
                    break;
                }
            }
    }
    return num;
}

bool starts_with(const wstring &s, const wstring &sub) {
    return s.find(sub) == 0;
}

bool ends_with(const wstring &s, const wstring &sub) {
    return s.rfind(sub) == (s.length() - sub.length());
}

template<class T>
T *get_or_null(vector<T *> *vec, int index) {
    if (index >= 0 && index < vec->size())
        return (*vec)[index];
    else
        return NULL;
}

void log(const string &msg) {
    LOGD("---> %s", msg.c_str());
}

void log(const wstring &msg) {
    LOGD("---> %s", ws2s(msg).c_str());
}

int u2l(wchar_t &s) {
    if (s == L'两')
        return 2;
    else {
        wstring _ws = L"零一二三四五六七八九十";
        return _ws.find(s);
    }
}

long poolS(stack<long> *s, long l) {
    if (s->empty())
        return 1;
    long q;
    long sum = 0L;
    while (!s->empty()) {
        q = s->top();
        if (q < l) {
            q = s->top();
            s->pop();
            sum += q;
        } else
            break;
    }
    return sum;
}

long parseChineseNumber(const wstring &text) {
    map<wchar_t, long> cUnit;
    cUnit[L'亿'] = 100000000L;
    cUnit[L'万'] = 10000L;
    cUnit[L'千'] = 1000L;
    cUnit[L'百'] = 100L;
    cUnit[L'十'] = 10L;
    stack<long> stk;
    for (int i = 0; i < text.size(); i++) {
        wchar_t c = text[i];
        long cu = cUnit[c];
        if (cu != 0) {
            stk.push(poolS(&stk, cu) * cu);
        } else {
            long n = u2l(c);
            stk.push(n);
        }
    }
    long sum = 0L;
    while (!stk.empty()) {
        sum += stk.top();
        stk.pop();
    }
    return sum;
}

long toNum(const wstring &text) {
    if (text[0] >= L'0' && text[0] <= L'9') {
        istringstream is(ws2s(text));
        long b;
        is >> b;
        if (b == 0 && text[0] != '0' && text.size() == 1)
            throw ("数字转换失败" + ws2s(text));
        else
            return b;
    } else {
        return parseChineseNumber(text);
    }
}

//map<string, string>转HashMap
jobject ToHashMap(JNIEnv *env, map<string, string> *pointsMap) {
    jobject HashMap = NULL;
    jclass class_hashmap = env->FindClass("java/util/HashMap");
    jmethodID hashmap_init = env->GetMethodID(class_hashmap, "<init>",
                                              "()V");
    HashMap = env->NewObject(class_hashmap, hashmap_init, "");
    jmethodID HashMap_put = env->GetMethodID(class_hashmap, "put",
                                             "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    map<string, string>::const_iterator it;
    map<string, string>::const_iterator end = pointsMap->end();
    for (it = pointsMap->begin(); it != end; ++it) {
        jstring key = s2js(env, it->first);
        jstring value = s2js(env, it->second);
        env->CallObjectMethod(HashMap, HashMap_put, key, value);
    }
    return HashMap;
}

#endif
