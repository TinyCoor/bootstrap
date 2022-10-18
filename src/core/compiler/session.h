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
#include "common/source_file.h"
namespace gfx::compiler {

class session {
public:
    session(const session_options_t& options, const path_list_t& source_files);

    virtual ~session();

    bool run();

    bool compile();

    void finalize();

    bool initialize();

    assembler& assembler();

    compiler::program& program();

    class terp &terp();

    gfx::result& result();

    emit_context_t& emit_context();

    source_file *add_source_file(const fs::path &path);

    void push_source_file(source_file* source);

    source_file* pop_source_file();

    std::vector<source_file*> source_files();

    source_file* current_source_file();

    [[nodiscard]] const session_options_t& options() const;

    source_file* find_source_file(const fs::path &path);

    ast_node_shared_ptr parse(const fs::path& path);

    ast_node_shared_ptr parse(source_file* source);

    void error(const std::string& code, const std::string& message, const source_location& location);

    void error(compiler::element* element, const std::string& code, const std::string& message,
               const source_location& location);

private:
    void raise_phase(session_compile_phase_t phase, const fs::path& source_file) const;

    void write_code_dom_graph(const fs::path& path);

private:
    class terp terp_;
    gfx::result r;
    class assembler assembler_;
    compiler::program program_;
    emit_context_t context_;
    session_options_t options_ {};
    std::stack<source_file*> source_file_stack_{};
    std::map<std::string, source_file> source_files_ {};
};
}
#endif // COMPILER_SESSION_H_
