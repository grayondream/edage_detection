cmake_minimum_required(VERSION 3.5)
project(leetecode)

include_directories(.)
include_directories(lib/include)
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. SOURCE_FILES)
set(LIB G:/tmp/interview/edage_detection/lib/lib)
add_executable(detect ${SOURCE_FILES})
target_link_libraries(detect ${LIB}/libpng.lib)