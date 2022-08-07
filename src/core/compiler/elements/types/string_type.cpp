//
// Created by 12132 on 2022/5/2.
//

#include "string_type.h"
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
string_type::string_type(block* parent)
	: composite_type(parent, composite_types_t::struct_type, nullptr, element_type_t::string_type)
{

}

bool string_type::on_initialize(result &r, compiler::program *program)
{
    symbol(program->make_symbol(parent_scope(), "string"));
    auto block_scope = parent_scope();

    auto u32_type = program->find_type_down("u32");
    auto address_type = program->find_type_down("address");

    auto length_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope, "length"), nullptr, true);
    length_identifier->type(u32_type);
    auto length_field = program->make_field(block_scope, length_identifier);

    auto capacity_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope, "capacity"), nullptr, true);
    capacity_identifier->type(u32_type);
    auto capacity_field = program->make_field(block_scope, capacity_identifier);

    auto data_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope,  "data"), nullptr, true);
    data_identifier->type(address_type);
    auto data_field = program->make_field(block_scope, data_identifier);

    fields().add(length_field);
    fields().add(capacity_field);
    fields().add(data_field);
	return composite_type::on_initialize(r, program);
}
}