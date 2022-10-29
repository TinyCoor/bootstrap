//
// Created by 12132 on 2022/5/8.
//

#include "cast.h"
#include "types/type.h"
#include "types/numeric_type.h"
#include "symbol_element.h"
#include "core/compiler/session.h"
#include "fmt/format.h"
#include "common/defer.h"
namespace gfx::compiler {
enum class cast_mode_t : uint8_t {
    noop,
    integer_truncate,
    integer_sign_extend,
    integer_zero_extend,
    float_extend,
    float_truncate,
    float_to_integer,
    integer_to_float,
};

cast::cast(compiler::module* module, block* parent, compiler::type* type, element* expr)
	: element(module, parent, element_type_t::cast), expression_(expr), type_(type)
{
}

element* cast::expression()
{
	return expression_;
}

compiler::type* cast::type()
{
	return type_;
}

bool cast::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
    result.type = type_;
	return result.type != nullptr;
}
//
// numeric casts
// ------------------------------------------------------------------------
// casting between two integers of the same size (s32 -> u32) is a no-op
// casting from a larger integer to a smaller integer (u32 -> u8) will truncate via move
// casting from smaller integer to larger integer (u8 -> u32) will:
//  - zero-extend if the source is unsigned
//  - sign-extend if the source is signed
// casting from float to an integer will round the float towards zero
// casting from an integer to a float will produce the floating point representation of the
//    integer, rounded if necessary
// casting from f32 to f64 is lossless
// casting from f64 to f32 will produce the closest possible value, rounded if necessary
// casting bool to and integer type will yield 1 or 0
// casting any integer type whose LSB is set will yield true; otherwise, false
//
bool cast::on_emit(compiler::session& session)
{
    if (expression_ == nullptr) {
       return true;
    }
    session.emit_context().indent = 4;
    defer({
        session.emit_context().indent = 0;
    });
    auto mode = cast_mode_t::noop;
    type_inference_result_t source_type;
    expression_->infer_type(session, source_type);
    auto source_number_class = source_type.type->number_class();
    auto source_size = source_type.type->size_in_bytes();
    auto target_number_class = type_->number_class();
    auto target_size = type_->size_in_bytes();
    if (source_number_class == type_number_class_t::none) {
        session.error(this, "C073", fmt::format("cannot cast from type: {}", source_type.name()),
            expression_->location());
        return false;
    } else if (target_number_class == type_number_class_t::none) {
        session.error(this, "C073", fmt::format("cannot cast to type: {}", type_->symbol()->name()),
            type_location_);
        return false;
    }

    auto& assembler = session.assembler();
    auto target_reg = assembler.current_target_register();
    auto instruction_block = assembler.current_block();

    if (source_number_class == type_number_class_t::integer
        &&  target_number_class == type_number_class_t::integer) {
        if (source_size == target_size) {
            mode = cast_mode_t::integer_truncate;
        } else if (source_size > target_size) {
            mode = cast_mode_t::integer_truncate;
        } else {
            auto source_numeric_type = dynamic_cast<compiler::numeric_type*>(source_type.type);
            if (source_numeric_type->is_signed()) {
                mode = cast_mode_t::integer_sign_extend;
            } else {
                mode = cast_mode_t::integer_zero_extend;
            }
        }
    } else if (source_number_class == type_number_class_t::floating_point
        &&  target_number_class == type_number_class_t::floating_point) {
        if (source_size == target_size) {
            mode = cast_mode_t::float_truncate;
        } else if (source_size > target_size) {
            mode = cast_mode_t::float_truncate;
        } else {
            mode = cast_mode_t::float_extend;
        }
    } else {
        if (source_number_class == type_number_class_t::integer) {
            mode = cast_mode_t::integer_to_float;
        } else {
            mode = cast_mode_t::float_to_integer;
        }
    }

    auto temp_reg = register_for(session, expression_);
    if (!temp_reg.valid) {
        return false;
    }
    assembler.push_target_register(temp_reg.reg);
    expression_->emit(session);
    assembler.pop_target_register();

    switch (mode) {
        case cast_mode_t::integer_truncate: {
            instruction_block->move_reg_to_reg(*target_reg, temp_reg.reg);
            break;
        }
        case cast_mode_t::integer_sign_extend: {
            instruction_block->moves_reg_to_reg(*target_reg, temp_reg.reg);
            break;
        }
        case cast_mode_t::integer_zero_extend: {
            instruction_block->movez_reg_to_reg(*target_reg, temp_reg.reg);
            break;
        }
        case cast_mode_t::float_extend:
        case cast_mode_t::float_truncate:
        case cast_mode_t::float_to_integer:
        case cast_mode_t::integer_to_float: {
            instruction_block->convert(*target_reg, temp_reg.reg);
            break;
        }
        default: {
            break;
        }
    }

    instruction_block->comment(
        fmt::format("cast<{}> from type {}", type_->symbol()->name(), source_type.name()),
        session.emit_context().indent);
    return true;
}

void cast::on_owned_elements(element_list_t &list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}

void cast::type_location(const source_location &loc)
{
    type_location_ = loc;
}

}