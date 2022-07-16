//
// Created by 12132 on 2022/4/20.
//

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include "common/result.h"
#include "terp.h"
#include "instruction_emitter.h"
#include <vector>

namespace gfx {
enum class segment_type_t {
	code,
	data,
	stack,
	constant,
};

enum class symbol_type_t {
	unknown,
	u8,
	u16,
	u32,
	u64,
	f32,
	f64,
	bytes,
};

static inline std::unordered_map<symbol_type_t, std::string_view> symbol_type_names = {
    {symbol_type_t::u8,      "u8"},
    {symbol_type_t::u16,     "u16"},
    {symbol_type_t::u32,     "u32"},
    {symbol_type_t::u64,     "u64"},
    {symbol_type_t::f32,     "f32"},
    {symbol_type_t::f64,     "f64"},
    {symbol_type_t::bytes,   "bytes"},
};

static inline std::string_view symbol_type_name(symbol_type_t type)
{
    auto it = symbol_type_names.find(type);
    if (it == symbol_type_names.end()) {
        return "unknown";
    }
    return it->second;
}

static inline size_t sizeof_symbol_type(symbol_type_t type)
{
	switch (type) {
		case symbol_type_t::u8:
			return 1u;
		case symbol_type_t::u16:
			return 2u;
		case symbol_type_t::u32:
			return 4u;
		case symbol_type_t::u64:
			return 8u;
		case symbol_type_t::f32:
			return 4u;
		case symbol_type_t::f64:
			return 8u;
		case symbol_type_t::bytes:
			return 8u;
		default:
			return 0;
	}
}
static inline symbol_type_t float_symbol_type_for_size(size_t size) {
	switch (size) {
		case 4:
			return symbol_type_t::f32;
		case 8:
			return symbol_type_t::f64;
		default:
			return symbol_type_t::unknown;
	}
}

static inline symbol_type_t integer_symbol_type_for_size(size_t size) {
	switch (size) {
		case 1:
			return symbol_type_t::u8;
		case 2:
			return symbol_type_t::u16;
		case 4:
			return symbol_type_t::u32;
		case 8:
			return symbol_type_t::u64;
		default:
			return symbol_type_t::unknown;
	}
}
static inline std::unordered_map<segment_type_t, std::string_view> segment_type_names = {
	{segment_type_t::code,     "code"},
	{segment_type_t::data,     "data"},
	{segment_type_t::stack,    "stack"},
	{segment_type_t::constant, "constant"},
};

static inline std::string_view segment_type_name(segment_type_t type)
{
	auto it = segment_type_names.find(type);
	if (it == segment_type_names.end()) {
		return "unknown";
	}
	return it->second;
}

struct symbol_t {
	symbol_t(const std::string& name, symbol_type_t type, uint64_t address, size_t size =0);
	~symbol_t() = default;

	uint64_t address;
	std::string name;
	symbol_type_t type;
	size_t size;
	union {
		double float_value;
		uint64_t int_value;
		void* byte_array_value;
	}value{};
};

using symbol_list_t = std::vector<symbol_t>;

struct segment_t {
	uint64_t address =0;
	uint64_t offset = 0;
	std::string name;
	segment_type_t type;
	bool initialized = false;
	segment_t(const std::string &name, segment_type_t type, uint64_t address);

	size_t size() const;

	symbol_t* symbol(const std::string& name);

    symbol_list_t symbols();

	symbol_t* symbol(const std::string& name, symbol_type_t type, size_t size = 0);
private:
	std::unordered_map<std::string, symbol_t> symbols_{};
};
using segment_list_t = std::vector<segment_t>;
class assembler {
public:
	explicit assembler(terp* terp);

	virtual ~assembler();

	bool assemble(result& r, std::istream& source);

	instruction_emitter& emitter();

	void define_data(uint8_t value);

	void define_data(uint16_t value);

	void define_data(uint32_t value);

	void define_data(uint64_t value);

	void define_data(float value);

	void define_data(double value);

	void define_string(const std::string& value);

	void location_counter(uint64_t value);

	uint64_t location_counter() const;

    segment_list_t segments() const;

    segment_t* segment(const std::string& name);

	segment_t* segment(const std::string &name, segment_type_t type, uint64_t address);

private:
	terp* terp_ = nullptr;
	instruction_emitter emitter_;
	uint64_t location_counter_;
	std::unordered_map<std::string, segment_t> segments_{};
};
}
#endif // ASSEMBLER_H_
