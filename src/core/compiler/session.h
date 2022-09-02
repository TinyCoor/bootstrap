//
// Created by 12132 on 2022/8/21.
//

#ifndef COMPILER_SESSION_H_
#define COMPILER_SESSION_H_

#include <vm/terp.h>
#include <vm/assembler.h>
#include "compiler_types.h"
#include "elements/program.h"
#include "elements/element_types.h"
#include "parser/ast.h"
#include "common/result.h"
namespace gfx::compiler {

class session {
public:
    session(const session_options_t& options, const path_list_t& source_files);

    virtual ~session();

    void finalize();

    bool initialize(result& r);

    assembler& assembler();

    compiler::program& program();

    bool compile(result& r);

    class terp &terp();

    [[nodiscard]] const path_list_t& source_files() const;

    [[nodiscard]] const session_options_t& options() const;

    ast_node_shared_ptr parse(result& r, const fs::path& source_file);

    void raise_phase(session_compile_phase_t phase, const fs::path& source_file);

private:
    void write_code_dom_graph(const fs::path& path);

private:
    class terp terp_;
    class assembler assembler_;
    compiler::program program_;
    path_list_t source_files_ {};
    session_options_t options_ {};
};
}
#endif // COMPILER_SESSION_H_
