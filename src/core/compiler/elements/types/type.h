//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "../element.h"
#include "../element_types.h"
namespace gfx::compiler {
class type : public element {
public:
    type(block* parent_scope, element_type_t type, compiler::symbol_element* name);

	~type() override;

    bool packed() const;

    void packed(bool value);

    size_t alignment() const;

    void alignment(size_t value);

    size_t size_in_bytes() const;

    bool initialize(result &r, compiler::program* program);

    compiler::symbol_element* symbol() const;

    void symbol(compiler::symbol_element* value);
protected:
	void size_in_bytes(size_t value);

    void on_owned_elements(element_list_t& list) override;

	virtual bool on_initialize(result& r, compiler::program* program);
private:
    bool packed_ = false;
    size_t alignment_ = 0;
	size_t size_in_bytes_ {};
    compiler::symbol_element *symbol_;
};

}
#endif // COMPILER_ELEMENTS_TYPE_H_
