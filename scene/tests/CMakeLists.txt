include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/dep/googletest/include)
link_directories("${PROJECT_SOURCE_DIR}/lib")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" " -U__STRICT_ANSI__")
message(STATUS "PROJECT_SOURCE_DIR= " "${PROJECT_SOURCE_DIR} " )

add_executable(ViewFrustum_test ViewFrustumTest.cpp ../ViewFrustum.cpp)
target_link_libraries(ViewFrustum_test gtest_main gtest)

add_executable(MVP_test MVPTest.cpp)
target_link_libraries(MVP_test gtest_main gtest)
