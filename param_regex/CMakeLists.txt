#指定cmake最低支持的版本（这是一条可选命令）
cmake_minimum_required(VERSION 3.4.1)
#设置本地动态库，编译生成动态库
#
add_library(
        #添加库的库名
        pr
        #添加库的类型  STATIC：静态库  SHARED ：动态库  MODULE：在使用dyld的系统有效，若不支持dyld，等同于SHARED
        SHARED
        #表示该库不会被默认构建
        #EXCLUDE_FROM_ALL  *.cpp
        src/main/jni/pr.cpp)
#查找系统库，日志输出文件log
find_library(
        log-lib
        log)
#需要连接或者编译库
target_link_libraries(
        pr
        ${log-lib})