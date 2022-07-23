//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
#define COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
#include "type.h"
#include "field.h"
namespace gfx::compiler {
class procedure_type : public type {
public:
	explicit procedure_type(element* parent, compiler::block* scope, const std::string& name);

	compiler::block* scope();

	field_map_t& returns();

	bool is_foreign() const;

	void is_foreign(bool value);

	field_map_t& parameters();

	[[maybe_unused]] type_map_t& type_parameters();

	procedure_instance_list_t& instances();

protected:
	bool on_initialize(result& r, compiler::program* program) override;

    bool on_emit(result& r, assembler& assembler, const emit_context_t& context)  override;
private:
	field_map_t returns_{};
	field_map_t parameters_{};
	type_map_t type_parameters_{};
	bool is_foreign_ = false;
	compiler::block* scope_ = nullptr;
	procedure_instance_list_t instances_ {};
};
}

#endif // COMPILER_ELEMENTS_PROCEDURE_TYPE_H_
