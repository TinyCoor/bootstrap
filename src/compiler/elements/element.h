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
	element(element* parent);

	virtual ~element();

	id_t id() const;

	bool fold(result& r);

	element* parent();

	attribute_map_t& attributes();

protected:
	virtual bool on_fold(result& result);

private:
	id_t id_;
	element* parent_ = nullptr;
	attribute_map_t attributes_ {};
};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
