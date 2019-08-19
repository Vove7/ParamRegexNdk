#ifndef PARAM_REGEX
#define PARAM_REGEX

#include "nodes.cpp"
#include "stringbuilder.h"
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <typeinfo>
#include "strutils.h"
#include <vector>

using namespace std;

class ParamRegex {
public:
    wstring regex;
    vector<RegNode *> *regNodeList = nullptr;
    unsigned long regIndex;
    stack<GroupNode *> groupStack;

    ParamRegex(string r) {
        regex = s2ws(r);
        regIndex = 0;
    }

    ~ParamRegex() {
        if (!regNodeList->empty()) {
            //删除第一个，next自动删除
            delete (*regNodeList)[0];
        }

        delete regNodeList;
    }

    void linkBack(vector<RegNode *> *list, RegNode *node) {

        RegNode *pre = get_or_null(list, list->size() - 1);
        if (pre != NULL) {
            pre->nextNode = node;
        }
        list->push_back(node);
    }

    void buildNode(StringBuilder<wchar_t> *sb, vector<RegNode *> *list) {
        wstring str = sb->ToString();
        if (str.size() != 0) {
            log(L"TextNode :" + str);
            TextNode *node = new TextNode(str);
            linkBack(list, node);
        }
        sb->Clear();
    }

    vector<RegNode *> *buildRegNodeList() {
        auto *list = new vector<RegNode *>();
        unsigned long l = regex.size();
        StringBuilder<wchar_t> sb;

        while (regIndex < l) {
            switch (regex[regIndex]) {
                case '(': { //进入group
                    buildNode(&sb, list);
                    regIndex++;
                    int b = regIndex;
                    GroupNode *group = new GroupNode();
                    groupStack.push(group);
                    group->setNodeList(buildRegNodeList());
                    groupStack.pop();
                    group->regText = regex.substr(b - 1, regIndex - b);

                    linkBack(list, group);
                }
                    break;

                case ')': //结束
                {
                    GroupNode *group = groupStack.top();
                    if (group == NULL) {
                        throw ("括号不匹配");
                    }
                    regIndex = group->buildMatchCount(++regIndex, regex);
                    buildNode(&sb, list);
                    return list;
                }
                    break;

                case '[': {
                    buildNode(&sb, list);
                    CharsNode *orNode = new CharsNode();
                    regIndex = orNode->buildOneCharNodes(regex, ++regIndex);
                    linkBack(list, orNode);
                }
                    break;

                case '|': {
                    buildNode(&sb, list);
                    regIndex++;
                    vector<RegNode *> *backList = buildRegNodeList();
                    OrNode *orNode = new OrNode();
                    GroupNode *preGroup = new GroupNode();
                    preGroup->subNodeList = new vector<RegNode *>(*list);
                    GroupNode *backGroup = new GroupNode();

                    backGroup->subNodeList = backList;
                    orNode->orList = new vector<RegNode *>();
                    linkBack(orNode->orList, preGroup);
                    linkBack(orNode->orList, backGroup);
                    list->clear();
                    linkBack(list, orNode);

                    if (groupStack.size() > 0)
                        return list;
                }
                    break;
                case '@': {
                    buildNode(&sb, list);
                    regIndex++;
                    ParamNode *paramNode = new ParamNode();
                    if (regex[regIndex] == '{') {
                        int index = regex.find('}', regIndex + 1);
                        if (index < 0) {
                            throw ("未匹配到'}' after @{ at index " + i2s(regIndex));
                        }
                        string pName = ws2s(regex.substr(regIndex + 1, index - regIndex - 1));
                        if (pName[0] == '#') {
                            paramNode->onlyNumber = true;
                            paramNode->name = pName.substr(1);
                        } else {
                            paramNode->name = pName;
                        }

                        log("param Name: " + paramNode->name);
                        paramNode->minMatchCount = 0;
                        regIndex = paramNode->buildMatchCount(index + 1, regex);
                        linkBack(list, paramNode);
                    }
                }
                    break;
                case '#': {
                    buildNode(&sb, list);
                    ParamNode *paramNode = new ParamNode();
                    paramNode->regText = L"#";
                    paramNode->onlyNumber = true;
                    regIndex = paramNode->buildMatchCount(++regIndex, regex);
                    linkBack(list, paramNode);
                }
                    break;
                case '%': {
                    buildNode(&sb, list);
                    ParamNode *paramNode = new ParamNode();
                    paramNode->regText = L"%";
                    paramNode->name = "%" + i2s(findParamIndex(regIndex));
                    paramNode->minMatchCount = 0;
                    paramNode->maxMatchCount = MAX_INT;
                    regIndex = paramNode->buildMatchCount(++regIndex, regex);
                    linkBack(list, paramNode);
                }
                    break;
                default:
                    if (regIndex + 1 < l) { //未超出
                        wstring s = L"*+?";
                        if (s.find(regex[regIndex + 1]) != string::npos) {
                            TextNode *singleCharNode = new TextNode(regex.substr(regIndex, 1));
                            regIndex = singleCharNode->buildMatchCount(++regIndex, regex);
                            buildNode(&sb, list);
                            linkBack(list, singleCharNode);
                        } else {
                            sb.Append(regex.substr(regIndex, 1));
                            regIndex++;
                        }
                    } else { //结尾
                        sb.Append(&regex[regIndex]);
                        buildNode(&sb, list);
                        regIndex++;
                    }
                    break;
            }
        }
        buildNode(&sb, list);
        return list;
    }

    int findParamIndex(int index) {
        int ii = 0;
        for (int i = 0; i != index; i++) {
            if (regex[i] == '%') {
                ii++;
            }
        }
        return ii;
    }

    map<string, string> *match(string text) {
        return match(s2ws(text));
    }

    map<string, string> *match(wstring text) {

        auto *matchList = new map<string, string>();

        int endIndex = 0;

        if (regNodeList == nullptr) {
            regNodeList = buildRegNodeList();
        }

        int size = static_cast<int>(regNodeList->size());
        log("NODE SIZE: " + i2s(size));
        for (int j = 0; j < size; j++) {
            RegNode *it = (*regNodeList)[j];
            endIndex = it->match(text, endIndex, it->nextNode);
            if (endIndex < 0) {
                return nullptr;
            }
            if (it->isParamNode()) {
                ParamNode *p = dynamic_cast<ParamNode *>(it);
                if (((ParamNode *) it)->onlyNumber) {
                    (*matchList)[p->name] = l2s(toNum(it->matchValue));
                } else {
                    (*matchList)[p->name] = ws2s(it->matchValue);
                }
            }
        }

        if (endIndex >= text.size()) {
            return matchList;
        }
        return NULL;
    }
};

#endif