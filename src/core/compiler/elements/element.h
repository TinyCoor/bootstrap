//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_H_
#define COMPILER_ELEMENTS_ELEMENT_H_
#include "common/id_pool.h"
#include "compiler/emit_context.h"
#include "common/result.h"
#include "common/source_location.h"
#include "element_types.h"
#include "vm/assembler.h"
namespace gfx::compiler {

class element {
public:
	element(block* parent, element_type_t type, element* parent_element = nullptr);

	virtual ~element();

	id_t id() const;

	element* fold(result& r, compiler::program* program);

	block* parent_scope();

    template <typename T>
    T* parent_element_as()
    {
        if (parent_element_ == nullptr) {
            return nullptr;
        }
        return dynamic_cast<T*>(parent_element_);
    }

    bool is_parent_element(element_type_t type);

    element* parent_element();

    const source_location& location() const;

    void location(const source_location& location);

    void parent_element(element* parent);

    bool emit(result& r, emit_context_t& context);

    bool is_constant() const;

    bool as_bool(bool &value);

    bool as_float(double &value);

    bool as_integer(uint64_t &value);

    bool as_string(std::string &value);

    virtual std::string label_name() const final;

	attribute_map_t& attributes();

	element_type_t element_type() const;

    void owned_elements(element_list_t& list);

    attribute* find_attribute(const std::string& name);

    compiler::type* infer_type(const compiler::program* program);

protected:
    virtual bool on_is_constant() const;

    virtual bool on_as_bool(bool &value) const;

    virtual bool on_as_float(double &value) const;

    virtual bool on_as_integer(uint64_t &value) const;

    virtual bool on_as_string(std::string &value) const;

    virtual void on_owned_elements(element_list_t& list);

    virtual bool on_emit(result& r, emit_context_t& context);

    virtual element *on_fold(result& result, compiler::program* program);

	virtual compiler::type* on_infer_type(const compiler::program* program);

private:
	id_t id_;
	block* parent_scope_ = nullptr;
    element* parent_element_ = nullptr;
    source_location location_ {};
    attribute_map_t attributes_ {};
	element_type_t element_type_ = element_type_t::element;

};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
