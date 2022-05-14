cmake_minimum_required(VERSION 3.21)
project(bootstrap CXX C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/fmt)
#add_subdirectory(extern/dyncall)
add_subdirectory(extern/gtest)

include_directories(${PROJECT_SOURCE_DIR}/src)
message(STATUS ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

# dot -Tpdf ast.dot -oast.pdf


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
        extern/dyncall
        extern/boost-1.79
)

#all unit test
get_property(test_targets GLOBAL PROPERTY UNIT_TEST_NAME)
add_custom_target(all-unit-tests ALL DEPENDS ${test_targets})
add_custom_command(TARGET all-unit-tests
        COMMENT "Exec All unit test"
        POST_BUILD
        COMMAND ctest -C $<CONFIGURATION> --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

set(COMMON_SRC src/common/ya_getopt.c
        src/common/id_pool.cpp
        )

set(VM_SRC
        src/vm/terp.cpp
        src/vm/instruction_emitter.cpp
        src/vm/instruction.cpp
        src/vm/instruction_cache.cpp
        src/vm/shared_library.cpp
        src/vm/assembler.cpp
        )

set(ELEMENT_SRC
        src/compiler/elements/attribute.cpp
        # operator
        src/compiler/elements/operator_base.cpp
        src/compiler/elements/unary_operator.cpp
        src/compiler/elements/binary_operator.cpp

        # literal
        src/compiler/elements/float_literal.cpp
        src/compiler/elements/integer_literal.cpp
        src/compiler/elements/boolean_literal.cpp
        src/compiler/elements/string_literal.cpp
        # type
        src/compiler/elements/type.cpp
        src/compiler/elements/any_type.cpp
        src/compiler/elements/element_types.cpp
        src/compiler/elements/string_type.cpp
        src/compiler/elements/composite_type.cpp
        src/compiler/elements/numeric_type.cpp
        src/compiler/elements/procedure_type.cpp

        src/compiler/elements/if_element.cpp
        src/compiler/elements/return_element.cpp
        src/compiler/elements/procedure_call.cpp
        src/compiler/elements/block.cpp
        src/compiler/elements/directive.cpp
        src/compiler/elements/element.cpp
        src/compiler/elements/expression.cpp
        src/compiler/elements/field.cpp
        src/compiler/elements/identifier.cpp
        src/compiler/elements/initializer.cpp
        src/compiler/elements/namespace_element.cpp
        src/compiler/elements/program.cpp

        src/compiler/elements/label.cpp
        src/compiler/elements/comment.cpp
        src/compiler/elements/statement.cpp
        src/compiler/elements/alias.cpp
        src/compiler/elements/cast.cpp
        src/compiler/elements/procedure_instance.cpp
        )

set(COMPILER_SRC
        src/compiler/scope.cpp
        src/compiler/symbol_table.cpp
        ${ELEMENT_SRC}
        src/compiler/bytecode_emitter.cpp)


set(PARSER_SRC
        src/parser/lexer.cpp
        src/parser/token.cpp
        src/parser/parser.cpp
        src/parser/ast.cpp
        src/parser/infix_parser.cpp
        src/parser/prefix_parser.cpp
        src/parser/ast_formatter.cpp)

add_executable(bootstrap
        src/main.cpp
        ${COMMON_SRC}
        ${VM_SRC}
        ${PARSER_SRC}
        ${COMPILER_SRC}
        )
target_link_directories(bootstrap PUBLIC lib)
target_link_libraries(bootstrap PUBLIC
        fmt
        dyncallback_s
        dyncall_s
        dynload_s
        )

add_custom_target(bootstrap-configured DEPENDS dummy-target bootstrap)
add_dependencies(bootstrap-configured all-unit-tests)
add_custom_command(
        TARGET bootstrap-configured
        COMMAND ${CMAKE_COMMAND} -E echo "placeholder")