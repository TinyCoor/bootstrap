//
// Created by 12132 on 2022/5/14.
//
#include "program.h"
#include "core/compiler/session.h"
#include "identifier.h"
#include "identifier_reference.h"
#include "symbol_element.h"
#include "types/procedure_type.h"
#include "procedure_call.h"
#include "fmt/format.h"
#include "common/defer.h"
namespace gfx::compiler {

procedure_call::procedure_call(compiler::module* module, block* parent, compiler::identifier_reference* reference,
    argument_list* args)
	: element(module, parent, element_type_t::proc_call),  arguments_(args), reference_(reference)
{

}
argument_list* procedure_call::arguments()
{
	return arguments_;
}

bool procedure_call::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
    auto identifier = reference_->identifier();
    if (identifier != nullptr) {
        auto proc_type = dynamic_cast<procedure_type*>(identifier->type());
        auto returns_list = proc_type->returns().as_list();
        result.type = returns_list.front()->identifier()->type();
        return result.type != nullptr;
    }
    return false;
}

bool procedure_call::on_emit(compiler::session &session)
{
    auto &context = session.emit_context();
    auto& assembler = session.assembler();
    context.indent = 4;
    defer({
          context.indent = 0;
    });
    auto instruction_block = assembler.current_block();
    auto identifier = reference_->identifier();
    auto init = identifier->initializer();
    if (init == nullptr) {
        return false;
    }
    auto procedure_type = init->procedure_type();

    if (arguments_ != nullptr) {
        arguments_->emit(session);
    }
    if (procedure_type->is_foreign()) {
        instruction_block->comment(fmt::format("foreign call: {}", identifier->symbol()->name()),
                                   context.indent);
        instruction_block->push_constant<uint16_t>(arguments_->elements().size());
        instruction_block->call_foreign(procedure_type->foreign_address());

    } else {
        instruction_block->call(assembler.make_label_ref(identifier->symbol()->name()));
    }
    auto target_reg = assembler.current_target_register();
    if (target_reg != nullptr) {
        if (!procedure_type->returns().as_list().empty()) {
            instruction_block->pop(*target_reg);
        }
    }
    return true;
}

compiler::identifier_reference* procedure_call::reference()
{
    return reference_;
}

void procedure_call::reference(compiler::identifier_reference* value)
{
    reference_ = value;
}

void procedure_call::on_owned_elements(element_list_t &list)
{
    if (arguments_ != nullptr) {
        list.emplace_back(arguments_);
    }
}

}