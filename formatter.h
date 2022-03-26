//
// Created by 12132 on 2022/3/26.
//

#ifndef BOOTSTRAP__FORMATTER_H_
#define BOOTSTRAP__FORMATTER_H_
#include <string>
namespace cfg {
class formatter {
public:
	static std::string dump_to_string(const void* data, size_t size);
};
}

#endif //BOOTSTRAP__FORMATTER_H_
