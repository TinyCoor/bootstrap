//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#define COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#include "type.h"
#include "field.h"
namespace gfx::compiler {
class composite_type  : public type {
public:
  	explicit composite_type(element* parent, const std::string name);

  	~composite_type() override;

	field_map_t& fields() {
		return fields_;
	}

	type_map_t& type_parameters() {
		return type_parameters_;
	}


private:
	field_map_t fields_{};
	type_map_t type_parameters_{};
};
}

#endif // COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
