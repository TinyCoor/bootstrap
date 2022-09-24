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
    type_number_class_t number_class;
};

using numeric_type_map_t = std::unordered_map<std::string, numeric_type_properties_t*>;

class numeric_type : public type {
public:
	explicit numeric_type(block* parent_scope, symbol_element* symbol, int64_t min, uint64_t max,
                          bool is_signed, type_number_class_t number_class);

	int64_t min() const;

	uint64_t max() const;

    bool is_signed() const;

    static std::string narrow_to_value(uint64_t value);

    static std::string narrow_to_value(double value);

	static type_list_t make_types(result& r, compiler::block* parent, compiler::program* program);
protected:
    static inline std::vector<numeric_type_properties_t> s_type_properties = {
        {"u8",   0,         UINT8_MAX,   1, false, type_number_class_t::integer},
        {"u16",  0,         UINT16_MAX,  2, false, type_number_class_t::integer},
        {"u32",  0,         UINT32_MAX,  4, false, type_number_class_t::integer},
        {"u64",  0,         UINT64_MAX,  8, false, type_number_class_t::integer},
        {"s8",   INT8_MIN,  INT8_MAX,    1, true, type_number_class_t::integer},
        {"s16",  INT16_MIN, INT16_MAX,   2, true, type_number_class_t::integer},
        {"s32",  INT32_MIN, INT32_MAX,   4, true, type_number_class_t::integer},
        {"s64",  INT64_MIN, INT64_MAX,   8, true, type_number_class_t::integer},
        {"f32",  0,         UINT32_MAX,  4, true, type_number_class_t::floating_point},
        {"f64",  0,         UINT64_MAX,  8, true, type_number_class_t::floating_point},
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
    type_number_class_t on_number_class() const override;

    type_access_model_t on_access_model() const override;

	bool on_initialize(result& r, compiler::program* program) override;

private:
	int64_t min_;
	uint64_t max_;
    bool is_signed_;
    type_number_class_t number_class_;
};
}
#endif // COMPILER_ELEMENTS_NUMERIC_TYPE_H_
