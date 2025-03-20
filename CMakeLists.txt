cmake_minimum_required(VERSION 3.13)
project(clop)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_VERBOSE_MAKEFILE 1)

find_package(CSOUND)
if(NOT CSOUND_INCLUDE_DIRS)
    message(FATAL_ERROR "Csound header files are required")
endif()

set(SRCS opcode.cpp)
find_package(OpenCL REQUIRED)

add_library(clop SHARED ${SRCS})

if(APPLE)
target_link_libraries(clop "-framework OpenCL")
else()
target_link_libraries(clop OpenCL)
endif()

target_include_directories(clop PRIVATE ${CSOUND_INCLUDE_DIR}
${CMAKE_SOURCE_DIR})
message(STATUS ${CSOUND_INCLUDE_DIR})
set_property(TARGET clop PROPERTY CXX_STANDARD 14)


