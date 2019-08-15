#ifndef REG_NODE
#define REG_NODE

#include "range.cpp"
#include "utils.cpp"
#include <exception>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

using namespace std;

#define MAX_INT 32767

class RegNode {

public:
    wstring regText;
    wstring matchValue;
    int minMatchCount;
    int maxMatchCount;
    RegNode *preNode = NULL;
    RegNode *nextNode = NULL;

    RegNode() {
        minMatchCount = 1;
        maxMatchCount = 1;
    }

    virtual bool isParamNode() {
        return false;
    }

    int buildMatchCount(int index, wstring s) {
        if (index >= s.size())
            return index;

        switch (s[index]) {
            case '*':
                minMatchCount = 0;
                maxMatchCount = MAX_INT;
                break;
            case '+':
                minMatchCount = 1;
                maxMatchCount = MAX_INT;
                break;
            case '?':
                minMatchCount = 0;
                if (isParamNode()) {
                    maxMatchCount = MAX_INT;
                }else {
                    maxMatchCount = 1;
                }
                break;
            case '{': {
                int endIndex = s.find('}', index + 1);
                if (endIndex < index)
                    throw ("require '}' after '{' at " + i2s(index));
                wstring text = s.substr(index + 1, endIndex - index - 1);//√
                if (text.find(',') != string::npos) { //{1,2} / {1,}
                    vector<string> ss;
                    split(ws2s(text), ss, ',');
                    minMatchCount = str_to_int(ss[0].c_str());
                    if (ss.size() == 1) //{1,}
                        maxMatchCount = MAX_INT;
                    else //{1,2}
                        maxMatchCount = str_to_int(ss[1].c_str());
                } else { //{2}
                    minMatchCount = str_to_int(ws2s(text).c_str());
                    maxMatchCount = minMatchCount;
                }
                return endIndex + 1;
            }
                break;
            default: {
                if (isParamNode()) {
                    minMatchCount = 0;
                    maxMatchCount = MAX_INT;
                } else {
                    minMatchCount = 1;
                    maxMatchCount = 1;
                }
                return index;
            }
                break;
        }
        return index + 1;
    }

    //返回-1 失败
    virtual int match(wstring s, int startIndex, RegNode *nextNode) {
        log("执行基类啦");
        return -1;
    }
};

class ParamNode : public RegNode {
public:
    string name;
    bool onlyNumber;

    ParamNode()
            : RegNode() {
        onlyNumber = false;
    }

    bool isParamNode() {
        return true;
    }

    //TODO 数字字符
    string numberValue() {
        return ws2s(matchValue);
    }

    int match(wstring s, int startIndex, RegNode *nextNode) {
        if (startIndex == s.size()) {
            if (minMatchCount == 0)
                return startIndex;

            else return -1;
        }
        if (onlyNumber)
            return matchNumber(s, startIndex);
        else
            return matchString(s, startIndex, nextNode);
    }

    int matchNumber(wstring s, int startIndex) {
        int l = s.size();
        wstring numArray = L"0123456789零一二两三四五六七八九十百千万";
        int endIndex = startIndex;

        while (endIndex < l) {
            int i = numArray.find(s[endIndex]);
            if (i != string::npos) {
                endIndex++;
            } else {
                break;
            }
        }
        if (startIndex == endIndex) {
            throw ("未匹配到数字");
        }
        matchValue = s.substr(startIndex, endIndex - startIndex);//√
        return endIndex;
    }

    int matchString(wstring s, int startIndex, RegNode *nextNode) {
        if (nextNode == NULL) {
            matchValue = s.substr(startIndex);
            return s.size();
        }
        int size = s.size();
        for (int i = startIndex + minMatchCount; i < size; i++) {
            //防止nextNode minMatchCount = 0
            RegNode *node = nextNode;
            int nextIndex = i;

            while (node != NULL) {
                if (node->minMatchCount == 0) { //检查未匹配
                    nextIndex = node->match(s, i, nextNode->nextNode);
                    if (nextIndex == i) //未匹配到
                        node = node->nextNode;
                    else
                        break; //匹配到
                } else {
                    nextIndex = node->match(s, i, node->nextNode);
                    break;
                }
            }

            if (nextIndex >= 0) {
                int matchCount = i - startIndex;
                if (matchCount < minMatchCount || matchCount > maxMatchCount) {
                    matchValue = L"";
                    LOGD("范围匹配失败 --> %d  not in (%d,%d)", matchCount, minMatchCount, maxMatchCount);
                    return -1;
                }
                matchValue = s.substr(startIndex, i - startIndex);//√
                return i;
            }
        }
        return -1;
    }

};

class OrNode : public RegNode {
public:
    vector<RegNode *> *orList = NULL;

    OrNode()
            : RegNode() {
    }

    int match(wstring s, int startIndex, RegNode *nextNode) {
        if (startIndex == s.size()) {
            if (minMatchCount == 0)
                return startIndex;
            else return -1;
        }
        int size = orList->size();
        for (int index = 0; index < size; index++) {
            RegNode *value = (*orList)[index];
            int endIndex = value->match(s, startIndex, NULL);
            if (endIndex >= 0) {
                matchValue = s.substr(startIndex, endIndex - startIndex);
                return endIndex;
            }
        }
        return -1;
    }
};

class CharsNode : public RegNode {
private:
    vector<Range> rangeList;

public:
    int buildOneCharNodes(wstring regex, int index) {
        int i = index;
        int size = regex.size();
        while (i < size) {
            wchar_t c = regex[i];
            if (Range('a', 'z').contains(c) || Range('A', 'Z').contains(c) ||
                Range('0', '9').contains(c)) {
                if (regex[i + 1] == '-') {
                    rangeList.emplace_back(regex[i++], regex[++i]);
                } else {
                    rangeList.emplace_back(c, c);
                }
            } else if (c == ']') {
                return buildMatchCount(++i, regex);
            } else {
                rangeList.emplace_back(c, c);
            }
            i++;
        }
        throw ("require ']' after '[' at " + i2s(i));
    }

    bool inRange(wchar_t c) {
        int size = rangeList.size();
        for (int i = 0; i < size; i++) {
            if (rangeList[i].contains(c)) {
                return true;
            }
        }
        return false;
    }

    int match(wstring s, int startIndex, RegNode *nextNode) {
        int size = s.size();
        if (startIndex >= size) {
            if (minMatchCount == 0)
                return startIndex;
            else
                return -1;
        }
        int endIndex = startIndex;

        for (int i = startIndex; i < size; i++) {
            if (i - startIndex >= maxMatchCount)
                break; //限制次数
            wchar_t c = s[i];
            if (inRange(c)) {
                endIndex = i + 1;
            } else {
                break;
            }
        }

        if (minMatchCount == 0 && startIndex == endIndex) {
            return startIndex;
        }

        int matchCount = endIndex - startIndex;
        if (matchCount < minMatchCount) {
            LOGD(" 匹配次数 --> %d < %d", matchCount, minMatchCount);
            return -1;
            //匹配失败
        }
        matchValue = s.substr(startIndex, endIndex - startIndex);
        return endIndex;
    }
};

class TextNode : public RegNode {
private:
    wstring text;

public:
    TextNode(wstring t)
            : RegNode() {
        text = t;
        regText = t;
    }

    TextNode(string t)
            : RegNode() {
        text = s2ws(t);
        regText = text;
    }

    int match(wstring s, int startIndex, RegNode *nextNode) {
        if (startIndex >= s.size()) {
            if (minMatchCount == 0)
                return startIndex;
            else
                return -1;
        }

        if (starts_with(s.substr(startIndex), text)) {
            matchValue = text;
            return startIndex + text.size();
        } else if (minMatchCount == 0) {
            matchValue = L"";
            return startIndex;
        } else
            return -1;
    }
};

/**
 * 被() 包起来的
 * @property subNodeList LinkedList<RegNode>
 */
class GroupNode : public RegNode {

public:
    vector<RegNode *> *subNodeList = NULL;

    void setNodeList(vector<RegNode *> *l) {
        subNodeList = l;
    }

    int match(wstring s, int startIndex, RegNode *nextNode) {
        if (startIndex == s.size()) {
            if (minMatchCount == 0)
                return startIndex;

            else return -1;
        }
        if (startIndex >= s.size() && minMatchCount != 0)
            return -1;

        int endIndex = startIndex;
        int matchCount = 0;
        while (endIndex < s.size() && matchCount < maxMatchCount) {
            bool result = true;
            int x = 0;
            for (int i = 0; i < subNodeList->size(); i++) {
                RegNode *node = (*subNodeList)[i];
                RegNode *next = get_or_null(subNodeList, x++);
                int partEndIndex = node->match(s, endIndex, next);
                if (partEndIndex < 0) {
                    result = false;
                    break;
                } else { //匹配失败
                    endIndex = partEndIndex;
                }
            }
            //一轮结束失败 && minMatchCount == 0
            if (!result && minMatchCount == 0) {
                LOGD("group未匹配到");
                return startIndex; //group未匹配到
            }
            if (result) { //匹配成功，继续向后匹配
                matchCount++;
                continue;
            } else { //第2+轮 失败
                if (matchCount >= minMatchCount) {
                    matchValue = s.substr(startIndex, endIndex - startIndex);
                    return endIndex;
                } else {
                    return -1;
                }
            }
        }
        matchValue = s.substr(startIndex, endIndex - startIndex);
        //        Vog.d(" 匹配次数 --> $matchCount")
        return endIndex;
    }
};

#endif