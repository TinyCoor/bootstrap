//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "element.h"
#include "element_types.h"
namespace gfx::compiler {
class type : public element {
public:
	type();

	~type() override;

	void add_field(const std::string& name, compiler::type* type,
		compiler::initializer* initializer);

	inline uint64_t min() const {
		return min_;
	}

	inline uint64_t max() const {
		return max_;
	}

	inline std::string name() const {
		return name_;
	}

	inline size_t field_count() const {
		return fields_.size();
	}

	bool remove_field(const std::string& name);

	compiler::field* find_field(const std::string& name);

private:
	uint64_t min_;
	uint64_t max_;
	std::string name_;
	field_map_t fields_ {};
};
}
#endif // COMPILER_ELEMENTS_TYPE_H_
