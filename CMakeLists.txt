# 项目名称
project(shitServer)
# cmake 允许的最小版本
cmake_minimum_required(VERSION 2.8)
# 头文件目录
include_directories(include)
# 查找src 所有员文件 保存操 DIR_SRC 中
aux_source_directory(./src DIR_SRCS)
# 库文件位置
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua-5.4.3/src/)

#-D xxx 宏定义    or  编译命令行
add_definitions(-std=c++11 -g)



# 指定生成目标
add_executable(shitServer ${DIR_SRCS})

# 库文件 多线性 库
find_package (Threads)
target_link_libraries (shitServer ${CMAKE_THREAD_LIBS_INIT})


# lua include 文件
include_directories(./3rd/lua-5.4.3/src/)
target_link_libraries(shitServer liblua.a)
target_link_libraries(shitServer dl)