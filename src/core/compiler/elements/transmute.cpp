//
// Created by 12132 on 2022/10/14.
//

#include "transmute.h"
#include "types/type.h"
#include "symbol_element.h"
#include "fmt/format.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
transmute::transmute(compiler::module* module, block* parent_scope, compiler::type* type, element* expr)
    : element(module, parent_scope, element_type_t::transmute), expression_(expr), type_(type)
{
}

bool transmute::on_emit(compiler::session &session)
{
    if (expression_ == nullptr) {
        return true;
    }
    type_inference_result_t source_type;
    expression_->infer_type(session, source_type);
    if (source_type.type->number_class() == type_number_class_t::none) {
        session.error(this, "C073", fmt::format("cannot transmute from type: {}", source_type.name()),
            expression_->location());
        return false;
    } else if (type_->number_class() == type_number_class_t::none) {
        session.error(this, "C073", fmt::format("cannot transmute to type: {}", type_->symbol()->name()),
            type_location_);
        return false;
    }

    auto& assembler = session.assembler();
    auto target_reg = assembler.current_target_register();
    auto instruction_block = assembler.current_block();

    auto temp_reg = register_for(session, expression_);
    if (!temp_reg.valid) {
        return false;
    }

    assembler.push_target_register(temp_reg.reg);
    expression_->emit(session);
    assembler.pop_target_register();

    instruction_block->move_reg_to_reg(*target_reg, temp_reg.reg);
    instruction_block->current_entry()->comment(fmt::format("transmute<{}>", type_->symbol()->name()),
        session.emit_context().indent);

    return true;
}

element* transmute::expression()
{
    return expression_;
}

compiler::type* transmute::type()
{
    return type_;
}

void transmute::on_owned_elements(element_list_t& list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}

bool transmute::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
    result.type = type_;
    return true;
}

void transmute::type_location(const source_location &loc)
{
    type_location_ = loc;
}
}