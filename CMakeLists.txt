cmake_minimum_required(VERSION 3.21)
project(bootstrap CXX C )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
#include_directories(extern/dyncall/dyncall/dyncall.h)
add_subdirectory(extern/fmt)
add_subdirectory(extern/gtest)
add_subdirectory(extern/dyncall-1.4)
add_subdirectory(extern/utf8proc-2.7.0)
include_directories(extern/fmt/include)
include_directories(extern/dyncall-1.4)
include_directories(extern/boost-1.79)
include_directories(extern/utf8proc-2.7.0)

# conan configuration
#set(CONAN_DISABLE_CHECK_COMPILER true)
#include(conanbuildinfo.cmake)
#conan_basic_setup(KEEP_RPATHS)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message(STATUS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<$<CONFIG:Release>: ${CMAKE_BINARY_DIR}/Release>)
#set(EXECUTABLE_OUTPUT_PATH  $<$<CONFIG:Release>: ${CMAKE_BINARY_DIR}/Release>)
#
if ($<CONFIG:Debug>)
    message(STATUS "Debug")

endif ()

include_directories(${PROJECT_SOURCE_DIR}/src)


# dot -Tpdf ast.dot -o ast.pdf
# ../../examples/example.gfx -Gast.dot -Hcode_dom.dot --verbose

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-private-field -Wno-unknown-pragmas -Wno-inconsistent-missing-override")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall" CACHE STRING "compile flags" FORCE)
else()
    message(STATUS "Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src/core)
add_executable(bootstrap
        src/bst/main.cpp
        src/common/ya_getopt.c
        src/common/string_extension.cpp)
target_include_directories(bootstrap PRIVATE src/core)
target_link_libraries(bootstrap compiler-rt)
enable_testing()
