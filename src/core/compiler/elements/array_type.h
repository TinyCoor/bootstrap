//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_ARRAY_TYPE_H_
#define COMPILER_ELEMENTS_ARRAY_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class array_type : public composite_type {
public:
	array_type(element* parent, const std::string& name, compiler::type* entry_type, size_t size);

	uint64_t size() const;

	void size(uint64_t value);

	compiler::type* entry_type();

protected:
	bool on_initialize(result& r, compiler::program* program) override;

private:
	uint64_t size_ = 0;
	compiler::type* entry_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ARRAY_TYPE_H_
