//
// Created by 12132 on 2022/4/20.
//

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include "common/result.h"
#include "terp.h"
#include "instruction_emitter.h"
#include <istream>

namespace gfx {
enum class segment_type_t {
	code,
	data,
	stack,
	constant,
};

static inline std::unordered_map<segment_type_t, std::string_view>  segment_type_names= {
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

struct segment_t {
	uint64_t size;
	uint64_t address;
	segment_type_t type;
};

struct symbol_t{
	uint64_t address;
	std::string name;
	segment_type_t type;
};


class assembler {
public:
	explicit assembler(terp* terp);

	virtual ~assembler();

	void symbol(const std::string& name, segment_type_t type, uint64_t address);

	bool assemble(result& r, std::istream& source);

	instruction_emitter& emitter();

	void define_data(uint8_t value);

	void define_data(uint16_t value);

	void define_data(uint32_t value);

	void define_data(uint64_t value);

	void define_data(float value);

	void define_data(double value);

	void define_string(const std::string& value);

	void segment(segment_type_t type, uint64_t address);

	void location_counter(uint64_t value);

	symbol_t* symbol(const std::string& name);

	uint64_t location_counter() const;

	segment_t* segment(segment_type_t type) ;
private:
	terp* terp_ = nullptr;
	instruction_emitter emitter_;
	uint64_t location_counter_;
	std::unordered_map<std::string, symbol_t> symbols_{};
	std::unordered_map<segment_type_t,segment_t> segments_{};
};
}
#endif // ASSEMBLER_H_
