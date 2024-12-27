cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME compra)
project(${PROJECT_NAME} VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")

if(UNIX)
    set(LIBEXT ".so")
    set(APPEXT "")
else()
    set(LIBEXT ".dll")
    set(APPEXT ".exe")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_FILES
    src/compra.cpp
)

add_library(compra SHARED ${SOURCE_FILES})
target_include_directories(compra PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(compra PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)