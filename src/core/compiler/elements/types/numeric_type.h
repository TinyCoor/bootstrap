//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#define COMPILER_ELEMENTS_NUMERIC_TYPE_H_
#include "type.h"
#include "vm/assembler.h"
namespace gfx::compiler {

struct numeric_type_properties_t {
    std::string name;
	int64_t min;
	uint64_t max;
	size_t size_in_bytes;
    bool is_signed = false;
};

using numeric_type_map_t = std::unordered_map<std::string, numeric_type_properties_t*>;

class numeric_type : public type {
public:
	explicit numeric_type(block* parent_scope, symbol_element* symbol, int64_t min, uint64_t max,
                          bool is_signed);

	int64_t min() const;

	uint64_t max() const;

    bool is_signed() const;

	symbol_type_t symbol_type() const;

    static std::string narrow_to_value(uint64_t value);

	static type_list_t make_types(result& r, compiler::block* parent, compiler::program* program);
protected:
    static inline std::vector<numeric_type_properties_t> s_type_properties = {
        {"u8",   0,         UINT8_MAX,   1},
        {"u16",  0,         UINT16_MAX,  2},
        {"u32",  0,         UINT32_MAX,  4},
        {"u64",  0,         UINT64_MAX,  8},
        {"s8",   INT8_MIN,  INT8_MAX,    1, true},
        {"s16",  INT16_MIN, INT16_MAX,   2, true},
        {"s32",  INT32_MIN, INT32_MAX,   4, true},
        {"s64",  INT64_MIN, INT64_MAX,   8, true},
        {"f32",  0,         UINT32_MAX,  4, true},
        {"f64",  0,         UINT64_MAX,  8, true},
    };

    static inline numeric_type_map_t s_types_map = {
		{"u8",       &s_type_properties[0]},
		{"u16",      &s_type_properties[1]},
		{"u32",      &s_type_properties[2]},
		{"u64",      &s_type_properties[3]},
		{"s8",       &s_type_properties[4]},
		{"s16",      &s_type_properties[5]},
		{"s32",      &s_type_properties[6]},
		{"s64",      &s_type_properties[7]},
		{"f32",      &s_type_properties[8]},
		{"f64",      &s_type_properties[9]},
	};

	bool on_initialize(result& r, compiler::program* program) override;

private:
	int64_t min_;
	uint64_t max_;
    bool is_signed_;
};
}
#endif // COMPILER_ELEMENTS_NUMERIC_TYPE_H_
