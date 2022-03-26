//
// Created by 12132 on 2022/3/26.
//

#include "terp.h"
namespace cfg {

terp::terp(uint32_t heap_size) : heap_size_(heap_size)
{

}

terp::~terp() {
	if (heap_) {
		delete heap_;
		heap_ = nullptr;
		heap_size_ = 0;
	}
}

bool terp::initialize(result& r) {
	heap_ = new uint64_t[heap_size_in_qwords()];

	registers_.pc = 0;
	registers_.fr = 0;
	registers_.sr = 0;
	registers_.sp = heap_size_in_qwords();
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.i[i] = 0;
		registers_.f[i] = 0.0;
	}
	return r.is_failed();
}

uint64_t terp::pop() {
	uint64_t value = heap_[registers_.sp];
	registers_.sp += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value) {
	registers_.sp -= sizeof(uint64_t);
	heap_[registers_.sp] = value;
}

const register_file_t& terp::register_file() const {
	return registers_;
}

size_t terp::heap_size() const {
	return heap_size_;
}

size_t terp::heap_size_in_qwords() const {
	return heap_size_ / sizeof(uint64_t);
}

}