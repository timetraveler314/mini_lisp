cmake_minimum_required(VERSION 3.24.2)

project(test_mini_lisp)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

aux_source_directory(../src TEST_SOURCES_ROOT)
aux_source_directory(../src/utils TEST_SOURCES_UTILS)
set (TEST_SOURCES ${TEST_SOURCES_ROOT} ${TEST_SOURCES_UTILS}
        ../src/utils/task.h)
set(TEST_SOURCES_EXCLUDE_MAIN "${TEST_SOURCES}")
list(FILTER TEST_SOURCES_EXCLUDE_MAIN EXCLUDE REGEX "../src/main.cpp")
add_executable(test_mini_lisp ${TEST_SOURCES_EXCLUDE_MAIN} test.cpp)
set_target_properties(
        test_mini_lisp
        PROPERTIES CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)
target_link_libraries(
        test_mini_lisp
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(test_mini_lisp)