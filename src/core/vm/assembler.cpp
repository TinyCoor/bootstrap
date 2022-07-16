//
// Created by 12132 on 2022/4/20.
//

#include "assembler.h"

namespace gfx {
symbol_t::symbol_t(const std::string& name,  symbol_type_t type, uint64_t address, size_t size)
	:  address(address), name(name) ,type(type), size(size)
{

}

segment_t::segment_t(const std::string &name, segment_type_t type, uint64_t address)
	:  address(address), name(name), type(type)
{}

symbol_t *segment_t::symbol(const std::string &name)
{
	auto it = symbols_.find(name);
	if (it == symbols_.end()) {
		return nullptr;
	}
	return &it->second;
}

size_t segment_t::size() const
{
	return offset;
}

symbol_t* segment_t::symbol(const std::string &name, symbol_type_t type, size_t size)
{
	auto type_size = size == 0 ? sizeof_symbol_type(type) : size;
	symbols_.insert(std::make_pair(name, symbol_t(name, type, address + offset, type_size)));
	offset += type_size;
	return symbol(name);
}

symbol_list_t segment_t::symbols()
{
    symbol_list_t list{};
    for (const auto& it : symbols_) {
        list.emplace_back(it.second);
    }
    return list;
}

assembler::assembler(terp* terp)
	: terp_(terp)
{
	location_counter_ = terp_->heap_vector(heap_vectors_t::program_start);
}

assembler::~assembler()
{

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
	auto heap_address = terp_->heap() + location_counter_;
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

segment_t *assembler::segment(const std::string &name, segment_type_t type, uint64_t address)
{
	segments_.insert(std::make_pair(name, segment_t(name, type, address)));
	return segment(name);
}

void assembler::location_counter(uint64_t value)
{
	location_counter_ = value;
}

uint64_t assembler::location_counter() const
{
	return location_counter_;
}

segment_t *assembler::segment(const std::string &name)
{
	auto it = segments_.find(name);
	if (it == segments_.end()) {
		return nullptr;
	}
	return &it->second;
}

segment_list_t assembler::segments() const
{
    segment_list_t list {};
    for (const auto& it: segments_) {
        list.push_back(it.second);
    }
    return list;
}

}