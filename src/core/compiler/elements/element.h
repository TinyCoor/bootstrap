//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_H_
#define COMPILER_ELEMENTS_ELEMENT_H_
#include "common/id_pool.h"
#include "common/result.h"
#include "element_types.h"
#include "vm/assembler.h"
namespace gfx::compiler {
class element {
public:
	element(element* parent, element_type_t type);

	virtual ~element();

	id_t id() const;

	bool fold(result& r);

	element* parent();

    bool emit(result& r, assembler& assembler);

    bool is_constant() const;

    bool as_bool(bool &value);

    bool as_float(double &value);

    bool as_integer(uint64_t &value);

    bool as_string(std::string &value);

	attribute_map_t& attributes();

	element_type_t element_type() const;

	compiler::type* infer_type(const compiler::program* program);

protected:
    virtual bool on_emit(result& r, assembler& assembler);

    virtual bool on_is_constant() const;

	virtual bool on_fold(result& result);

    virtual bool on_as_bool(bool &value) const;

    virtual bool on_as_float(double &value) const;

    virtual bool on_as_integer(uint64_t &value) const;

    virtual bool on_as_string(std::string &value) const;

	virtual compiler::type* on_infer_type(const compiler::program* program);

private:
	id_t id_;
	element* parent_ = nullptr;
	element_type_t element_type_ = element_type_t::element;
    attribute_map_t attributes_ {};
};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
