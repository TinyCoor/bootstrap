//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_ARRAY_TYPE_H_
#define COMPILER_ELEMENTS_ARRAY_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class array_type : public type {
public:
	array_type(const std::string& name, compiler::type* element_type);

	uint64_t size() const
	{
		return size_;
	}

	void size(uint64_t value)
	{
		size_ = value;
	}

	compiler::type* element_type() {
		return element_type_;
	}

private:
	uint64_t size_ = 0;
	compiler::type* element_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ARRAY_TYPE_H_
