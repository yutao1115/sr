include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/dep/googletest/include)
link_directories("${PROJECT_SOURCE_DIR}/lib")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" " -U__STRICT_ANSI__")
message(STATUS "PROJECT_SOURCE_DIR= " "${PROJECT_SOURCE_DIR} " )

add_executable(Vectors_test VectorsTest.cpp)
target_link_libraries(Vectors_test gtest_main gtest)

add_executable(Matrix_test MatrixTest.cpp)
target_link_libraries(Matrix_test gtest_main gtest)

add_executable(Quat_test QuaternionTest.cpp)
target_link_libraries(Quat_test gtest_main gtest)


