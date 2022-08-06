//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_H_
#define COMPILER_ELEMENTS_ELEMENT_H_
#include "common/id_pool.h"
#include "compiler/emit_context.h"
#include "common/result.h"
#include "element_types.h"
#include "vm/assembler.h"
namespace gfx::compiler {

class element {
public:
	element(block* parent, element_type_t type, element* parent_element = nullptr);

	virtual ~element();

	id_t id() const;

	bool fold(result& r);

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

	compiler::type* infer_type(const compiler::program* program);

protected:
    virtual bool on_emit(result& r, emit_context_t& context);

    virtual bool on_is_constant() const;

	virtual bool on_fold(result& result);

    virtual bool on_as_bool(bool &value) const;

    virtual bool on_as_float(double &value) const;

    virtual bool on_as_integer(uint64_t &value) const;

    virtual bool on_as_string(std::string &value) const;

	virtual compiler::type* on_infer_type(const compiler::program* program);

private:
	id_t id_;
	block* parent_scope_ = nullptr;
    element* parent_element_ = nullptr;
	element_type_t element_type_ = element_type_t::element;
    attribute_map_t attributes_ {};
};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
