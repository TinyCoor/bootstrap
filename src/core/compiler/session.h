//
// Created by 12132 on 2022/8/21.
//

#ifndef COMPILER_SESSION_H_
#define COMPILER_SESSION_H_
#include <cstdio>
#include <string>
#include <filesystem>
#include <vm/assembly_listing.h>
#include "elements/element_types.h"
#include "parser/ast.h"
#include "common/result.h"
namespace gfx::compiler {
namespace fs = std::filesystem;
using path_list_t = std::vector<fs::path>;

enum class session_compile_phase_t : uint8_t {
    start,
    success,
    failed
};

using session_compile_callback = std::function<void (session_compile_phase_t, const fs::path&)>;

struct session_options_t {
    bool verbose = false;
    fs::path ast_graph_file;
    fs::path dom_graph_file;
    session_compile_callback compile_callback;
};

class session {
public:
    session(const session_options_t& options, const path_list_t& source_files);

    virtual ~session();

    void finalize();

    assembly_listing& listing();

    const path_list_t& source_files() const;

    const session_options_t& options() const;

    void post_processing(compiler::program* program);

    ast_node_shared_ptr parse(result& r, const fs::path& source_file);

    void raise_phase(session_compile_phase_t phase, const fs::path& source_file);

private:
    void write_code_dom_graph(compiler::program* program, const fs::path& path);

private:
    path_list_t source_files_ {};
    session_options_t options_ {};
    assembly_listing listing_ {};
};
}
#endif // COMPILER_SESSION_H_
