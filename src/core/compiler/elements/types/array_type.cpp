//
// Created by 12132 on 2022/5/2.
//

#include "array_type.h"
#include "core/compiler/session.h"
#include "fmt/format.h"
#include "pointer_type.h"
#include "../symbol_element.h"
namespace gfx::compiler {

std::string array_type::name_for_array(compiler::type* entry_type, size_t size)
{
    return fmt::format("__array_{}_{}__", entry_type->symbol()->name(), size);
}

array_type::array_type(compiler::module* module, block* parent, compiler::block* scope,
                       compiler::type* entry_type,size_t size)
	:   composite_type(module, parent, composite_types_t::struct_type, scope, nullptr,  element_type_t::array_type),
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

bool array_type::on_initialize(compiler::session& session)
{
    auto program = &session.program();
    auto &builder = program->builder();
    auto type_symbol = builder.make_symbol(parent_scope(), name_for_array(entry_type_, size_));
    symbol(type_symbol);
    type_symbol->parent_element(this);

    auto block_scope = scope();

    auto u8_type = program->find_type(qualified_symbol_t{.name =  "u8"});
    auto u32_type = program->find_type(qualified_symbol_t{.name = "u32"});
    auto type_info_type = program->find_type(qualified_symbol_t{.name = "type"});

    auto flags_identifier =  builder.make_identifier(block_scope,
                                                     builder.make_symbol(block_scope,"flags"), nullptr);
    flags_identifier->type(u8_type);
    auto flags_field =  builder.make_field(block_scope, flags_identifier);

    auto length_identifier =  builder.make_identifier(block_scope,
        builder.make_symbol(block_scope,"length"), nullptr);
    length_identifier->type(u32_type);
    auto length_field =  builder.make_field(block_scope, length_identifier);

    auto capacity_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(block_scope,"capacity"), nullptr);
    capacity_identifier->type(u32_type);
    auto capacity_field =  builder.make_field(block_scope, capacity_identifier);

    auto element_type_identifier =  builder.make_identifier(block_scope,
        builder.make_symbol(block_scope, "element_type"), nullptr);

    element_type_identifier->type(type_info_type);
    auto element_type_field =  builder.make_field(block_scope, element_type_identifier);

    auto data_identifier =  builder.make_identifier(block_scope,
        builder.make_symbol(block_scope, "data"), nullptr);
    data_identifier->type( builder.make_pointer_type(session, block_scope, u8_type));
    auto data_field =  builder.make_field(block_scope, data_identifier);

    auto& field_map = fields();
    field_map.add(flags_field);
    field_map.add(length_field);
    field_map.add(capacity_field);
    field_map.add(element_type_field);
    field_map.add(data_field);
    return composite_type::on_initialize(session);
}

type_access_model_t array_type::on_access_model() const
{
    return type_access_model_t::pointer;
}
}
