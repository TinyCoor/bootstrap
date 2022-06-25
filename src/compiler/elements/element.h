//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_H_
#define COMPILER_ELEMENTS_ELEMENT_H_
#include <common/id_pool.h>
#include <common/result.h>
#include "element_types.h"
namespace gfx::compiler {
class element {
public:
	element(element* parent, element_type_t type);

	virtual ~element();

	id_t id() const;

	bool fold(result& r);

	element* parent();

	attribute_map_t& attributes();

	element_type_t element_type() const;

	compiler::type* infer_type(const compiler::program* program);

protected:
	virtual bool on_fold(result& result);

	virtual compiler::type* on_infer_type(const compiler::program* program);

private:
	id_t id_;
	element* parent_ = nullptr;
	attribute_map_t attributes_ {};
	element_type_t element_type_ = element_type_t::element;
};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
