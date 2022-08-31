//
// Created by 12132 on 2022/4/14.
//

#include "bytecode_emitter.h"
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
bytecode_emitter::bytecode_emitter( const bytecode_emitter_options_t& options)
	: terp_(options.heap_size, options.stack_size)
{
}

bytecode_emitter::~bytecode_emitter() = default;

bool bytecode_emitter::initialize(result& r)
{
	return terp_.initialize(r);
}

bool bytecode_emitter::compile(result& r, compiler::session& session)
{
    compiler::program program_element(&terp_);
    if (program_element.compile(r, session)) {

    }
	return !r.is_failed();
}


}