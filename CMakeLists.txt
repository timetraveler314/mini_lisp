cmake_minimum_required(VERSION 3.24.2)

project(mini_lisp VERSION 0.0.1)

# detect git branch
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PROJECT_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# compile date and time
execute_process(
        COMMAND date "+%b %d %Y"
        OUTPUT_VARIABLE DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND date "+%H:%M:%S"
        OUTPUT_VARIABLE TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(cxxopts REQUIRED)

aux_source_directory(src SOURCES_ROOT)
aux_source_directory(src/utils SOURCES_UTILS)
aux_source_directory(src/modes SOURCES_MODES)
set (SOURCES ${SOURCES_ROOT} ${SOURCES_UTILS} ${SOURCES_MODES})

configure_file(src/version.h.in "${CMAKE_SOURCE_DIR}/src/version.h" @ONLY)

add_executable(mini_lisp ${SOURCES})
set_target_properties(
  mini_lisp
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
             RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
             RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
if(MSVC)
  target_compile_options(mini_lisp PRIVATE /utf-8 /Zc:preprocessor)
endif()

add_subdirectory(test)

#set(SOURCES_EXCLUDE_MAIN "${SOURCES}")
#list(FILTER SOURCES_EXCLUDE_MAIN EXCLUDE REGEX "src/main.cpp")
#add_executable(test_mini_lisp ${SOURCES_EXCLUDE_MAIN} test/test.cpp)
#set_target_properties(
#        test_mini_lisp
#        PROPERTIES CXX_STANDARD 20
#        CXX_STANDARD_REQUIRED ON)