include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/dep/googletest/include)
link_directories("${PROJECT_SOURCE_DIR}/lib")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" " -U__STRICT_ANSI__")
message(STATUS "PROJECT_SOURCE_DIR= " "${PROJECT_SOURCE_DIR} " )

add_executable(ResAlloc_test ResAllocTest.cpp )
target_link_libraries(ResAlloc_test gtest_main gtest)

add_executable(MeshBuilder_test MeshBuilderTest.cpp ../MeshBuilder.cpp ../Mesh.cpp ../ResRegistry.cpp ../ResourceState.cpp)
target_link_libraries(MeshBuilder_test gtest_main gtest)

