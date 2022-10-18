//
// Created by 12132 on 2022/5/29.
//

#ifndef COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
#define COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
#include "type.h"

namespace gfx::compiler {
class unknown_type : public type {
public:
    unknown_type(compiler::module* module, block* parent_scope, compiler::symbol_element* symbol);

    bool is_pointer() const;

    void is_pointer(bool value);

	bool is_array() const;

	void is_array(bool value);

	size_t array_size() const;

	void array_size(size_t value);

protected:
	bool on_initialize(compiler::session& session) override;

private:
	size_t array_size_ = 0;
	bool is_array_ = false;
    bool is_pointer_ = false;
};
}

#endif // COMPILER_ELEMENTS_UNKNOWN_TYPE_H_
