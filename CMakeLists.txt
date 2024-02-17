cmake_minimum_required(VERSION 3.27)

project(core)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_library(core STATIC
        src/core/engine.cpp
        src/core/abi.cpp
        src/core/camera.cpp
        src/core/camera.h
        src/core/entity.cpp
        src/core/geometry.h
        src/core/geometry.h
        src/inbound/buffer.c
        src/inbound/buffer.h
        src/inbound/client.h
)

#Boost 1.84
#CGAL
#Seastar
#Skia

target_include_directories(core
        PUBLIC
        "/usr/local/include/boost"
        "/usr/local/include/CGAL"
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)


#include_directories(${Boost_INCLUDE_DIRS})
#target_link_libraries(world ${Boost_LIBRARIES})


#set_target_properties (world PROPERTIES OUTPUT_NAME ${name})
#
#add_dependencies (apps ${target})