cmake_minimum_required(VERSION 3.21)
project(bootstrap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/fmt)

include_directories(${PROJECT_SOURCE_DIR}/src)
message(STATUS ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-private-field -Wno-unknown-pragmas -Wno-inconsistent-missing-override")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall" CACHE STRING "compile flags" FORCE)
else()
    message(STATUS "Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()
#
#if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#    conan_basic_setup()
#else()
#    message(WARNING "The file conanbuildinfo.cmake does not exist you should run conan install . -s build_type=Debug --install-folder=build")
#endif ()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


enable_testing()
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "")
macro(add_unit_test target test_name args)
    set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${CMAKE_CURRENT_BINARY_DIR}/${target})
    add_test(NAME ${test_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} ${args})
endmacro()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

add_custom_target(dummy-target ALL DEPENDS custom-output)
add_custom_command(OUTPUT custom-output COMMAND ${CMAKE_COMMAND} -E echo DEPENDS always-rebuild)
add_custom_command(OUTPUT always-rebuild COMMAND ${CMAKE_COMMAND} -E echo )


#find-package
#find_package(FMT REQUIRED)

include_directories(
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
        ${FMT_INCLUDE_DIRS}
        extern/fmt/include
)

#all unit test
get_property(test_targets GLOBAL PROPERTY UNIT_TEST_NAME)
add_custom_target(all-unit-tests ALL DEPENDS ${test_targets})
add_custom_command(TARGET all-unit-tests
        COMMENT "Exec All unit test"
        POST_BUILD
        COMMAND ctest -C $<CONFIGURATION> --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

set(VM_SRC
        src/vm/terp.cpp
        src/vm/instruction_emitter.cpp
        src/vm/instruction.cpp
        src/vm/instruction_cache.cpp
        )

set(LEXER_SRC
        src/lexer/lexer.cpp
        src/lexer/token.cpp)

set(PARSER_SRC
        src/parser/scope.cpp
        src/parser/symbol_table.cpp
        src/parser/parser.cpp
        src/parser/ast/ast.cpp
        src/parser/ast/ast_formatter.cpp)

add_executable(bootstrap
        src/main.cpp
        src/compiler.cpp
        ${VM_SRC}
        ${PARSER_SRC}
        ${LEXER_SRC}
        )
target_link_libraries(bootstrap PUBLIC
        fmt
        )

add_custom_target(bootstrap-configured DEPENDS dummy-target bootstrap)
add_dependencies(bootstrap-configured all-unit-tests)
add_custom_command(
        TARGET bootstrap-configured
        COMMAND ${CMAKE_COMMAND} -E echo "placeholder")