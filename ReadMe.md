[![](https://jitpack.io/v/Vove7/ParamRegexNdk.svg)](https://jitpack.io/#Vove7/ParamRegexNdk)

# ParamRegexEngine-NDK

匹配形如 `@{city}的?天气` 的'正则式'

> NDK版[ParamRegexEngine](https://github.com/Vove7/ParamRegexEngine)，用于加快解析速度。

> 在新建实例下，性能约Kotlin版两倍。在大量测试用例下性能表现更优。

## 需求

- 匹配全字符
- 主要解析中文，不包括特殊字符，使用懒匹配
- 支持正则基本功能 [] () | *  +  ? 等
- @{param} 匹配任意字符，赋值到param
- #{num} 匹配数字 赋值到num, (num 自动转数字类型)
- 另两个符号： '%' 匹配大于0的字符串； '#'匹配长度大于1的数字(包括大小写) 
- 任意可限制匹配长度 ，限制标志：* + ? {1,2} {2} {1,}


如：(帮我)?@{text}第#{index}个  点击第二个 匹配返回：{index:2, text:点击}

### 测试用例：

- (123)?
- 123|abc
- [a-z123]*
- 12345
- [abc]*
- %123 
- [0-9]+
- abc?#
- (123)?abc
- #%
- @{text}123%
- (帮我)?@{text}第@{#index}个


> 写的匆忙，未考虑情况较多。欢迎指正

