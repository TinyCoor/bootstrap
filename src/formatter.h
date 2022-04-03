//
// Created by 12132 on 2022/3/26.
//

#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <string>
namespace gfx {

class formatter {
public:
	static std::string Hex(const void* data, size_t size);
};
}

#endif // FORMATTER_H_
