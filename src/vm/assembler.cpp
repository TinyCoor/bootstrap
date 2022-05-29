//
// Created by 12132 on 2022/4/20.
//

#include "assembler.h"
namespace gfx {

assembler::assembler(terp* terp)
	: terp_(terp)
{
	location_counter_ = terp_->heap_vector(heap_vectors_t::program_start);
}

assembler::~assembler()
{

}

void assembler::symbol(const std::string &name, segment_type_t type, uint64_t address)
{
	symbols_.insert(std::make_pair(name,
		symbol_t{
			.address = address,
			.name = name,
			.type = type,
		}));
}
bool assembler::assemble(result &r, std::istream &source)
{
	return false;
}
instruction_emitter& assembler::emitter()
{
	return emitter_;
}

void assembler::define_data(uint8_t value)
{
	auto heap_address =terp_->heap() + location_counter_;
	*heap_address = value;
	location_counter_ += sizeof(uint8_t);
}

void assembler::define_data(uint16_t value)
{
	auto heap_address =terp_->heap() + location_counter_;
	*heap_address = value;
	location_counter_ += sizeof(uint16_t);
}

void assembler::define_data(uint32_t value)
{
	auto heap_address =terp_->heap() + location_counter_;
	*heap_address = value;
	location_counter_ += sizeof(uint32_t);
}
void assembler::define_data(uint64_t value)
{
	auto heap_address =terp_->heap() + location_counter_;
	*heap_address = value;
	location_counter_ += sizeof(uint64_t);
}
void assembler::define_data(float value)
{
	auto heap_address = reinterpret_cast<uint32_t*>(terp_->heap() + location_counter_);
	*heap_address = static_cast<uint32_t>(value);
	location_counter_ += sizeof(uint32_t);
}
void assembler::define_data(double value)
{
	auto heap_address = reinterpret_cast<uint64_t *>(terp_->heap() + location_counter_);
	*heap_address = static_cast<uint64_t>(value);
	location_counter_ += sizeof(uint64_t);
}
void assembler::define_string(const std::string &value)
{
	auto heap_address =terp_->heap() + location_counter_;
	for (auto c : value) {
		*heap_address++ = static_cast<uint8_t>(c);
	}
	location_counter_ += value.length();
}
void assembler::segment(segment_type_t type, uint64_t address)
{
	segments_.insert(std::make_pair(
		type,
		segment_t{
			.size = 0,
			.address = address,
			.type = type,
		}));
}
void assembler::location_counter(uint64_t value)
{
	location_counter_ = value;
}
uint64_t assembler::location_counter() const
{
	return location_counter_;
}

symbol_t *assembler::symbol(const std::string &name)
{
	auto it = symbols_.find(name);
	if (it == symbols_.end()) {
		return nullptr;
	}
	return &it->second;
}

segment_t *assembler::segment(segment_type_t type)
{
	auto it = segments_.find(type);
	if (it == segments_.end()) {
		return nullptr;
	}
	return &it->second;
}

}