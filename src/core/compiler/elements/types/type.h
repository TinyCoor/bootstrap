//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "../../compiler_types.h"
#include "../element.h"
#include "../element_types.h"
namespace gfx::compiler {
class type : public element {
public:
    type(compiler::module* module, block* parent_scope, element_type_t type, compiler::symbol_element* name);

	~type() override;

    bool packed() const;

    void packed(bool value);

    size_t alignment() const;

    void alignment(size_t value);

    size_t size_in_bytes() const;

    compiler::symbol_element* symbol() const;

    type_access_model_t access_model() const;

    type_number_class_t number_class() const;

    void symbol(compiler::symbol_element* value);

    bool type_check(compiler::type* other);

    bool initialize(compiler::session& session);
protected:
	void size_in_bytes(size_t value);

    virtual bool on_type_check(compiler::type* other);

    virtual type_access_model_t on_access_model() const;

    virtual type_number_class_t on_number_class() const;

    void on_owned_elements(element_list_t& list) override;

	virtual bool on_initialize(compiler::session& session);

private:
    bool packed_ = false;
    size_t alignment_ = 0;
	size_t size_in_bytes_ {};
    compiler::symbol_element *symbol_;
};

}
#endif // COMPILER_ELEMENTS_TYPE_H_
