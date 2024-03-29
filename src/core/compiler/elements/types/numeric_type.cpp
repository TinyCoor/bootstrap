//
// Created by 12132 on 2022/5/2.
//

#include "numeric_type.h"
#include "common/bytes.h"
#include "../symbol_element.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
numeric_type::numeric_type(compiler::module* module, block* parent, symbol_element* symbol,  int64_t min, uint64_t max,
                           bool is_signed, type_number_class_t number_class)
	: type(module, parent, element_type_t::numeric_type, symbol), min_(min), max_(max),
        is_signed_(is_signed) , number_class_(number_class)
{

}

int64_t numeric_type::min() const
{
	return min_;
}

uint64_t numeric_type::max() const
{
	return max_;
}

type_list_t numeric_type::make_types(compiler::session& session, compiler::block* parent)
{
	type_list_t list {};
    auto &builder = session.builder();
	for (const auto& props : s_type_properties) {
		auto type = builder.make_numeric_type(parent, props.name, props.min, props.max, props.is_signed,
                                               props.number_class);
		type->initialize(session);
        session.scope_manager().add_type_to_scope(type);
	}
	return list;
}

bool numeric_type::on_initialize(compiler::session& session)
{
	auto it = s_types_map.find(symbol()->name());
	if (it == s_types_map.end()) {
		return false;
	}
    alignment(it->second->size_in_bytes);
	size_in_bytes(it->second->size_in_bytes);
	return true;
}

std::string numeric_type::narrow_to_value(uint64_t value)
{
    size_t start_index = 0;
    size_t end_index = 4;
    if (is_sign_bit_set(value)) {
        end_index = 8;
        start_index = 4;
    }
    auto signed_value = static_cast<int64_t>(value);
    for (size_t i = start_index; i < end_index; i++) {
        auto& props = s_type_properties[i];
        if (props.is_signed) {
            if (signed_value >= props.min && signed_value <= static_cast<int64_t>(props.max)) {
                return props.name;
            }
        } else {
            if (value >= static_cast<uint64_t>(props.min) && value <= props.max) {
                return props.name;
            }
        }
    }
    return "u32";
}

bool numeric_type::is_signed() const
{
    return is_signed_;
}

type_number_class_t numeric_type::on_number_class() const
{
    return number_class_;
}

type_access_model_t numeric_type::on_access_model() const
{
    return type_access_model_t::value;
}

std::string numeric_type::narrow_to_value(double value)
{
    if (value < -3.4e+38 || value > 3.4e+38) {
        return "f64";
    } else if (value >= -3.4e+38 && value <=3.4e+38) {
        return "f32";
    } else {
        return "unknown";
    }
}

bool numeric_type::on_type_check(compiler::type *other)
{
    auto other_numeric_type = dynamic_cast<compiler::numeric_type*>(other);
    if (other_numeric_type == nullptr) {
        return false;
    }

    if (symbol()->name() == other_numeric_type->symbol()->name()) {
        return true;
    }

    if (other_numeric_type->number_class()  == type_number_class_t::floating_point) {
        if (number_class_ == type_number_class_t::floating_point) {
            return other_numeric_type->size_in_bytes() <= size_in_bytes();
        }
    }

    if (is_signed() && other_numeric_type->is_signed()) {
        return other_numeric_type->size_in_bytes() < size_in_bytes();
    }

    return other_numeric_type->size_in_bytes() <= size_in_bytes();
}

}