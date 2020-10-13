cmake_minimum_required(VERSION 3.4.1)

file(GLOB SRC_LIST "*.cpp" "src/main/cpp/*.cpp")
add_library( hello SHARED  ${SRC_LIST})
find_library(log-lib log)
target_link_libraries( hello ${log-lib} )