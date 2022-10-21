//
// Created by 12132 on 2022/5/2.
//

#include "string_type.h"
#include "pointer_type.h"
#include "../../session.h"
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
string_type::string_type(compiler::module* module, block* parent, block* scope)
	: composite_type(module, parent, composite_types_t::struct_type, scope, nullptr, element_type_t::string_type)
{

}

bool string_type::on_initialize(compiler::session& session)
{
    auto program = &session.program();
    auto &builder = session.builder();
    symbol(builder.make_symbol(parent_scope(), "string"));
    auto block_scope = scope();

    auto u32_type = session.scope_manager().find_type(qualified_symbol_t{.name = "u32"});
    auto u8_type = session.scope_manager().find_type({.name = "u8"});

    auto length_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(block_scope, "length"), nullptr);
    length_identifier->type(u32_type);
    auto length_field = builder.make_field(block_scope, length_identifier);

    auto capacity_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(block_scope, "capacity"), nullptr);
    capacity_identifier->type(u32_type);
    auto capacity_field = builder.make_field(block_scope, capacity_identifier);

    auto data_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(block_scope,  "data"), nullptr);
    data_identifier->type(builder.make_pointer_type(block_scope, u8_type));
    auto data_field = builder.make_field(block_scope, data_identifier);

    fields().add(length_field);
    fields().add(capacity_field);
    fields().add(data_field);
	return composite_type::on_initialize(session);
}

type_access_model_t string_type::on_access_model() const
{
    return type_access_model_t::pointer;
}
}