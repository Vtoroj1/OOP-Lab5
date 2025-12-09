cmake_minimum_required(VERSION 3.20)

project(Task5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

add_executable(memo main.cpp)

add_executable(run_test tests.cpp)

target_link_libraries(run_test gtest_main)

target_include_directories(memo PRIVATE .)
target_include_directories(run_test PRIVATE .)
add_test(NAME Task5Tests COMMAND run_test)