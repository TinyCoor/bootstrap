//
// Created by 12132 on 2022/5/3.
//

#include "any_type.h"
#include "pointer_type.h"
#include "core/compiler/session.h"

namespace gfx::compiler {
any_type::any_type(compiler::module* module, block * parent, block* scope)
	: composite_type(module, parent, composite_types_t::struct_type, scope, nullptr, element_type_t::any_type)
{

}

[[maybe_unused]] compiler::type *any_type::underlying_type()
{
	return underlying_type_;
}

[[maybe_unused]] void any_type::underlying_type(compiler::type *value)
{
	underlying_type_ = value;
}

bool any_type::on_initialize(compiler::session& session)
{
    auto program = &session.program();
    auto &builder = program->builder();
    symbol(builder.make_symbol(parent_scope(), "any"));
    auto block_scope = scope();

    auto type_info_type = program->find_type(qualified_symbol_t{.name = "type"});
    auto u8_type = program->find_type(qualified_symbol_t{.name = "u8"});

    auto type_info_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(parent_scope(), "type_info"), nullptr);

    type_info_identifier->type(type_info_type);
    auto type_info_field = builder.make_field(block_scope, type_info_identifier);

    auto data_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(parent_scope(), "data"), nullptr);

    data_identifier->type(builder.make_pointer_type(session, block_scope, u8_type));
    auto data_field = builder.make_field(block_scope, data_identifier);

    fields().add(type_info_field);
    fields().add(data_field);
    return composite_type::on_initialize(session);
}

type_number_class_t any_type::on_number_class() const
{
    return type_number_class_t::integer;
}
type_access_model_t any_type::on_access_model() const
{
    return type_access_model_t::value;
}

}