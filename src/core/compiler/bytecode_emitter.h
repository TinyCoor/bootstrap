//
// Created by 12132 on 2022/4/14.
//

#ifndef BYTECODE_EMITTER_H_
#define BYTECODE_EMITTER_H_
#include <string>
#include <cstdint>
#include "vm/terp.h"
#include <filesystem>
#include "session.h"
namespace gfx::compiler {
class program;
namespace fs = std::filesystem;
struct bytecode_emitter_options_t {
	size_t heap_size = 0;
	size_t stack_size = 0;
};

class bytecode_emitter {
public:
	explicit bytecode_emitter(const bytecode_emitter_options_t &options);

	virtual ~bytecode_emitter();

    bool initialize(result &r);

	bool compile(result &r, compiler::session& session);

private:
	terp terp_;
};

}
#endif // BYTECODE_EMITTER_H_
