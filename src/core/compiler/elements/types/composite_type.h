//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#define COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#include "type.h"
#include "core/compiler/elements/field.h"
namespace gfx::compiler {
class composite_type : public type {
public:
  	explicit composite_type(block* parent_scope, composite_types_t type,
       block* scope, compiler::symbol_element *symbol,
       element_type_t element = element_type_t::composite_type);

	field_map_t& fields();

    block* scope();

	type_map_t& type_parameters();

	composite_types_t type() const;

protected:
    bool on_is_constant() const override;

	bool on_initialize(result& r, compiler::program* program) override;

private:
    block* scope_ = nullptr;
	field_map_t fields_{};
	composite_types_t type_;
	type_map_t type_parameters_{};
};
}

#endif // COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
