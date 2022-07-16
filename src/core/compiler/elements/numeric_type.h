//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#define COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#include "type.h"
#include "vm/assembler.h"
namespace gfx::compiler {

struct numeric_type_properties_t {
	int64_t min;
	uint64_t max;
	size_t size_in_bytes;
};

using numeric_type_map_t = std::unordered_map<std::string, numeric_type_properties_t>;

class numeric_type : public type {
public:
	explicit numeric_type(element* parent, const std::string& name, int64_t min, uint64_t max);

	int64_t min() const;

	uint64_t max() const;

	symbol_type_t symbol_type() const;

	static type_list_t make_types(result& r, element* parent);
protected:
	static inline numeric_type_map_t s_types_map = {
		{"bool",    {0,         1,           1}},
		{"u8",      {0,         UINT8_MAX,   1}},
		{"u16",     {0,         UINT16_MAX,  2}},
		{"u32",     {0,         UINT32_MAX,  4}},
		{"u64",     {0,         UINT64_MAX,  8}},
		{"s8",      {INT8_MIN,  INT8_MAX,    1}},
		{"s16",     {INT16_MIN, INT16_MAX,   2}},
		{"s32",     {INT32_MIN, INT32_MAX,   4}},
		{"s64",     {INT64_MIN, INT64_MAX,   8}},
		{"f32",     {0,         UINT32_MAX,  4}},
		{"f64",     {0,         UINT64_MAX,  8}},
		{"address", {0,         UINTPTR_MAX, 8}},
	};

	bool on_initialize(result& r) override;

private:
	int64_t min_;
	uint64_t max_;
};
}
#endif // COMPILER_ELEMENTS_NUMERIC_TYPE_H_
