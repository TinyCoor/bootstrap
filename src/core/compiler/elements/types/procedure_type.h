//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
#define COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
#include "type.h"
#include "core/compiler/elements/field.h"
namespace gfx::compiler {
class procedure_type : public type {
public:
	explicit procedure_type(block* parent_scope, compiler::block* scope, symbol_element* symbol);

	compiler::block* scope();

	field_map_t& returns();

	bool is_foreign() const;

	void is_foreign(bool value);

	field_map_t& parameters();

	[[maybe_unused]] type_map_t& type_parameters();

	procedure_instance_list_t& instances();

    uint64_t foreign_address() const;

    void foreign_address(uint64_t value);

protected:
    bool on_is_constant() const override;

    type_access_model_t on_access_model() const override;

    void on_owned_elements(element_list_t& list) override;

    bool on_emit(result& r, emit_context_t& context)  override;

    bool on_initialize(result& r, compiler::program* program) override;

private:
	field_map_t returns_{};
	field_map_t parameters_{};
	type_map_t type_parameters_{};
	bool is_foreign_ = false;
    uint64_t foreign_address_ = 0;
	compiler::block* scope_ = nullptr;
	procedure_instance_list_t instances_ {};
};
}

#endif // COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
