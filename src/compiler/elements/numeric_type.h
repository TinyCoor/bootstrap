//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#define COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#include "type.h"
namespace gfx::compiler {

class numeric_type : public type {
public:
	explicit numeric_type(element* parent, const std::string& name, int64_t min, uint64_t max);

	inline int64_t min() const
	{
		return min_;
	}

	inline uint64_t max() const
	{
		return max_;
	}
private:
	int64_t min_;
	uint64_t max_;
};
}
#endif // COMPILER_ELEMENTS_NUMERIC_TYPE_H_
