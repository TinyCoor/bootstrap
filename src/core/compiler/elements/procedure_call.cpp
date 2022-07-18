//
// Created by 12132 on 2022/5/14.
//
#include "program.h"
#include "procedure_type.h"
#include "procedure_call.h"
namespace gfx::compiler {

procedure_call::procedure_call(element* parent, class identifier* identifier, argument_list* args)
	: element(parent, element_type_t::proc_call),  arguments_(args) ,identifier_(identifier)
{

}
argument_list* procedure_call::arguments()
{
	return arguments_;
}
compiler::identifier *procedure_call::identifier()
{
	return identifier_;
}

compiler::type *procedure_call::on_infer_type(const compiler::program *program)
{
	auto proc_type = dynamic_cast<procedure_type*>(identifier_->type());
	auto returns_list = proc_type->returns().as_list();
	return returns_list.front()->identifier()->type();
}
bool procedure_call::on_emit(result &r, assembler &assembler)
{
    auto instruction_block = assembler.current_block();

    if (arguments_ != nullptr) {
        arguments_->emit(r, assembler);
    }
    instruction_block->call(identifier()->name());

    return true;
}
}