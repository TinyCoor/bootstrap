//
// Created by 12132 on 2022/5/29.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
#include "type.h"

namespace gfx::compiler {
class unknown_type : public type {
public:
    unknown_type(element* parent, const std::string& name, bool is_array, size_t size);

	bool is_array() const;

	void is_array(bool value);

	size_t array_size() const;

	void array_size(size_t value);

protected:
	bool on_initialize(result& r) override;

private:
	size_t array_size_ = 0;
	bool is_array_ = false;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
