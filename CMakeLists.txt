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

add_library(libclop SHARED ${SRCS})

if(APPLE)
target_link_libraries(libclop "-framework OpenCL")
else()
target_link_libraries(libclop OpenCL)
endif()

target_include_directories(libclop PRIVATE ${CSOUND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
set_property(TARGET libclop PROPERTY CXX_STANDARD 14)


