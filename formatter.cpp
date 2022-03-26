//
// Created by 12132 on 2022/3/26.
//

#include "formatter.h"
#include <fmt/format.h>
#include <sstream>
namespace cfg{

std::string cfg::formatter::dump_to_string(const void *data, size_t size) {
	auto *buf = (unsigned char *)data;
	std::stringstream stream;
	for (size_t i = 0u; i < size; i += 16u) {
		stream << fmt::format("{:06x}: ", i);
		for (size_t j = 0; j < 16u; ++j) {
			if (i + j < size) {
				stream << fmt::format("{:02x} ", buf[i + j]);
			}else {
				stream << "    ";
			}
		}
		stream << " ";
		for (size_t j = 0u; j < 16u ; ++j){
			if (i + j < size) {
				stream << (char)(isprint(buf[i + j]) ? buf[i + j] : '.');
			}
		}
		stream << '\n';
	}
	return stream.str();
}

}