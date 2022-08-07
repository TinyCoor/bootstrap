cmake_minimum_required(VERSION 3.21)
project(compiler-rt CXX C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src)
message(STATUS ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

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

set(COMMON_SRC
        ../common/id_pool.cpp)

set(VM_SRC
        vm/terp.h                           vm/terp.cpp
        vm/label.h                          vm/label.cpp
        vm/symbol.h                         vm/symbol.cpp
        vm/segment.h                        vm/segment.cpp
        vm/assembler.h                      vm/assembler.cpp
        vm/instruction.h                    vm/instruction.cpp
        vm/stack_frame.h                    vm/stack_frame.cpp
        vm/block_entry.h                    vm/block_entry.cpp
        vm/shared_library.h                 vm/shared_library.cpp
        vm/assembly_listing.h               vm/assembly_listing.cpp
        vm/instruction_builder.h
        vm/instruction_block.h              vm/instruction_block.cpp
        vm/instruction_cache.h              vm/instruction_cache.cpp
        )

set(ELEMENT_SRC
        compiler/elements/element_types.cpp

        # operator
        compiler/elements/operator_base.cpp
        compiler/elements/unary_operator.cpp
        compiler/elements/binary_operator.cpp

        # literal
        compiler/elements/float_literal.cpp
        compiler/elements/integer_literal.cpp
        compiler/elements/boolean_literal.cpp
        compiler/elements/string_literal.cpp
# types
        compiler/elements/types/type.cpp
        compiler/elements/types/tuple_type.cpp
        compiler/elements/types/unknown_type.cpp
        compiler/elements/types/any_type.cpp
        compiler/elements/types/type_info.cpp
        compiler/elements/types/string_type.cpp
        compiler/elements/types/composite_type.cpp
        compiler/elements/types/numeric_type.cpp
        compiler/elements/types/procedure_type.cpp
        compiler/elements/types/struct_type.cpp
        compiler/elements/types/array_type.cpp
        compiler/elements/types/namespace_type.cpp

# elements
        compiler/elements/attribute.cpp

        compiler/elements/if_element.cpp
        compiler/elements/return_element.cpp
        compiler/elements/symbol_element.cpp
        compiler/elements/procedure_call.cpp
        compiler/elements/block.cpp
        compiler/elements/directive.cpp
        compiler/elements/element.cpp
        compiler/elements/expression.cpp
        compiler/elements/field.cpp
        compiler/elements/identifier.cpp
        compiler/elements/initializer.cpp
        compiler/elements/namespace_element.cpp
        compiler/elements/program.cpp

        compiler/elements/label.cpp
        compiler/elements/comment.cpp
        compiler/elements/statement.cpp
        compiler/elements/alias.cpp
        compiler/elements/cast.cpp

        compiler/elements/procedure_instance.cpp

        compiler/elements/argument_list.cpp
        compiler/elements/import.cpp
        compiler/extern.cpp
        compiler/emit_context.cpp
        compiler/element_map.cpp
        )

set(COMPILER_SRC
        ${ELEMENT_SRC}
        compiler/code_dom_formatter.cpp
        compiler/bytecode_emitter.cpp
       )

set(PARSER_SRC
        parser/lexer.cpp
        parser/token.cpp
        parser/parser.cpp
        parser/ast.cpp
        parser/infix_parser.cpp
        parser/prefix_parser.cpp
        parser/ast_formatter.cpp)

add_library(compiler-rt SHARED
        ${COMMON_SRC}
        ${VM_SRC}
        ${PARSER_SRC}
        ${COMPILER_SRC}
        )
target_link_directories(compiler-rt PUBLIC ../../lib)
target_link_libraries(compiler-rt PUBLIC
        fmt
        dyncallback_s
        dyncall_s
        dynload_s
        )
#set_target_properties(compiler-rt PUBLIC )

add_custom_target(bootstrap-configured DEPENDS dummy-target bootstrap)
add_dependencies(bootstrap-configured all-unit-tests)
add_custom_command(
        TARGET bootstrap-configured
        COMMAND ${CMAKE_COMMAND} -E echo "placeholder")