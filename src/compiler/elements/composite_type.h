//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#define COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
#include "type.h"
#include "field.h"
namespace gfx::compiler {

enum class composite_types_t {
	enum_type,
	union_type,
	struct_type,
};

class composite_type : public type {
public:
  	explicit composite_type(element* parent, composite_types_t type, const std::string name);

	field_map_t& fields() {
		return fields_;
	}

	type_map_t& type_parameters() {
		return type_parameters_;
	}

	composite_types_t type() const {
		return type_;
	}

private:
	field_map_t fields_{};
	composite_types_t type_;
	type_map_t type_parameters_{};
};
}

#endif // COMPILER_ELEMENTS_COMPOSITE_TYPE_H_
