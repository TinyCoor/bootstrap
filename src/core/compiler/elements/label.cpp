//
// Created by 12132 on 2022/5/3.
//

#include "label.h"
#include "compiler/session.h"
namespace gfx::compiler {
label::label(compiler::module* module, block* parent, const std::string& name)
	: element(module, parent, element_type_t::label), name_(name)
{

}

std::string label::name() const
{
	return name_;
}
bool label::on_is_constant() const
{
    return true;
}

bool label::on_emit(session &session)
{
    auto& assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    instruction_block->blank_line();
    instruction_block->label(assembler.make_label(name_));
    return true;
}
}