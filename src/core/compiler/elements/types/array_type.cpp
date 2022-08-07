//
// Created by 12132 on 2022/5/2.
//

#include "array_type.h"
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
array_type::array_type(block* parent,  compiler::symbol_element* symbol, compiler::type* entry_type, size_t size)
	:   composite_type(parent,composite_types_t::struct_type, symbol, element_type_t::array_type),
        size_(size), entry_type_(entry_type)
{
}

uint64_t array_type::size() const
{
	return size_;
}

void array_type::size(uint64_t value)
{
	size_ = value;
}

compiler::type* array_type::entry_type()
{
	return entry_type_;
}

bool array_type::on_initialize(result &r, compiler::program* program)
{
    auto block_scope = parent_scope();

    auto u8_type = program->find_type_down("u8");
    auto u32_type = program->find_type_down("u32");
    auto type_info_type = program->find_type_down("type");
    auto address_type = program->find_type_down("address");

    auto flags_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope,"flags"), nullptr, true);
    flags_identifier->type(u8_type);
    auto flags_field = program->make_field(block_scope, flags_identifier);

    auto length_identifier = program->make_identifier(block_scope,
          program->make_symbol(block_scope,"length"), nullptr, true);
    length_identifier->type(u32_type);
    auto length_field = program->make_field(block_scope, length_identifier);

    auto capacity_identifier = program->make_identifier(block_scope,
         program->make_symbol(block_scope,"capacity"), nullptr, true);
    capacity_identifier->type(u32_type);
    auto capacity_field = program->make_field(block_scope, capacity_identifier);

    auto element_type_identifier = program->make_identifier(block_scope,
         program->make_symbol(block_scope, "element_type"), nullptr, true);

    element_type_identifier->type(type_info_type);
    auto element_type_field = program->make_field(block_scope, element_type_identifier);

    auto data_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope, "data"), nullptr, true);

    data_identifier->type(address_type);
    auto data_field = program->make_field(block_scope, data_identifier);

    auto& field_map = fields();
    field_map.add(flags_field);
    field_map.add(length_field);
    field_map.add(capacity_field);
    field_map.add(element_type_field);
    field_map.add(data_field);
    return composite_type::on_initialize(r, program);
}
}
