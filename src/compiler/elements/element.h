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
	element();

	virtual ~element();

	id_t id() const;

	bool fold(result& r);

	bool remove_directive(const std::string& name);

	bool remove_attribute(const std::string& name);

	directive* find_directive(const std::string& name);

	attribute* find_attribute(const std::string& name);
protected:
	virtual bool on_fold(result& result);

private:
	id_t id_;
	directive_map_t directives_ {};
	attribute_map_t attributes_ {};
};
}

#endif // COMPILER_ELEMENTS_ELEMENT_H_
