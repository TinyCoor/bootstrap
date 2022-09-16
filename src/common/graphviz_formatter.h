//
// Created by 12132 on 2022/9/14.
//

#ifndef COMMON_GRAPHVIZ_FORMATTER_H_
#define COMMON_GRAPHVIZ_FORMATTER_H_
#include <string>
namespace gfx {
class graphviz_formatter {
public:
    static std::string escape_chars(const std::string& value);
};
}

#endif //BOOTSTRAP_SRC_COMMON_GRAPHVIZ_FORMATTER_H_
