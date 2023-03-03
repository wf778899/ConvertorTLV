project(Test_TLV)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


add_executable(${PROJECT_NAME} Test_TLV.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::GTest TLV)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MT")

add_test(TLV_gtests ${PROJECT_NAME})