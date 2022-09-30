//
// Created by 12132 on 2022/9/2.
//

#ifndef CORE_COMPILER_COMPILER_TYPES_H_
#define CORE_COMPILER_COMPILER_TYPES_H_
#include <filesystem>
#include <vector>
#include <string>
namespace gfx::compiler{
namespace fs = std::filesystem;
using path_list_t = std::vector<fs::path>;
class session;

enum class type_access_model_t {
    none,
    value,
    pointer
};

enum class type_number_class_t {
    none,
    integer,
    floating_point,
};

enum class identifier_usage_t : uint8_t {
    heap = 1,
    stack
};

enum class session_compile_phase_t : uint8_t {
    start,
    success,
    failed
};

using session_compile_callback = std::function<void (session_compile_phase_t, const fs::path&)>;

struct session_options_t {
    bool verbose = false;
    size_t heap_size = 0;
    size_t stack_size = 0;
    fs::path full_path;
    fs::path ast_graph_file;
    fs::path dom_graph_file;
    session_compile_callback compile_callback;
};

}
#endif //CORE_COMPILER_COMPILER_TYPES_H_
