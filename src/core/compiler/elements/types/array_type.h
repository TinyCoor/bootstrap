//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_ARRAY_TYPE_H_
#define COMPILER_ELEMENTS_ARRAY_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class array_type : public composite_type {
public:
	array_type(compiler::module* module, block* parent_scope, compiler::block* scope,
               compiler::type* entry_type, size_t size);

	uint64_t size() const;

	void size(uint64_t value);

	compiler::type* entry_type();

    static std::string name_for_array(compiler::type* entry_type, size_t size);
protected:
    type_access_model_t on_access_model() const override;

	bool on_initialize(compiler::session& session) override;

private:
	uint64_t size_ = 0;
	compiler::type* entry_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ARRAY_TYPE_H_
