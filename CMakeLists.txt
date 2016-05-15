cmake_minimum_required(VERSION 3.0)

project(sr CXX C)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

message(STATUS "CMAKE_SYSTEM_NAME  = " ${CMAKE_SYSTEM_NAME}  )
message(STATUS "PROJECT_SOURCE_DIR = " ${PROJECT_SOURCE_DIR}  )
message(STATUS "PROJECT_BINARY_DIR = " ${PROJECT_BINARY_DIR}  )

set(CXX_FLAGS
 -D_WIN32
 -D_FILE_OFFSET_BITS=64 
 -Wall
 -march=native
 -fms-extensions
 -std=c++14
 )

 
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/apps)
include_directories(${PROJECT_SOURCE_DIR}/util)
include_directories(${PROJECT_SOURCE_DIR}/math)
include_directories(${PROJECT_SOURCE_DIR}/primitives)
include_directories(${PROJECT_SOURCE_DIR}/scene)
include_directories(${PROJECT_SOURCE_DIR}/resource)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

set(base_SRCS
  main.cpp
  loopWin32.cpp
  Global.cpp
  WindowInput.cpp
  Profiler.cpp
  PipeLine.cpp
  scene/CameraRig.cpp
  scene/ViewFrustum.cpp
  scene/Transform.cpp
  resource/ResourceState.cpp
  resource/ResRegistry.cpp
  resource/Mesh.cpp
  resource/MeshBuilder.cpp
  )

link_directories("${PROJECT_SOURCE_DIR}/lib")
add_executable(sr ${base_SRCS})
target_link_libraries(sr gdi32)

add_subdirectory(math/tests)
add_subdirectory(scene/tests)
add_subdirectory(resource/tests)

 
