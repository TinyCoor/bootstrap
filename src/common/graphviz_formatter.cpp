//
// Created by 12132 on 2022/9/14.
//

#include "graphviz_formatter.h"
namespace gfx {
std::string graphviz_formatter::escape_chars(const std::string& value)
{
    std::string buffer;
    for (const auto &c : value) {
        if (c == '\"') {
            buffer += "\\\"";
        } else if (c == '{') {
            buffer += "\\{";
        } else if (c == '}') {
            buffer += "\\}";
        } else if (c == '.') {
            buffer += "\\.";
        } else {
            buffer += c;
        }
    }
    return buffer;
}
}