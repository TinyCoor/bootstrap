//
// Created by 12132 on 2022/9/11.
//

#include "colorizer.h"
#include <fmt/format.h>
#include <sstream>

namespace gfx {
std::string gfx::colorizer::colorize_range(const std::string &text,
                                                     size_t begin,
                                                     size_t end,
                                                     gfx::term_colors_t color) noexcept
{
    std::stringstream  color_stream;
    for (size_t i = 0; i < text.length() ; ++i) {
        if (i == begin) {
            color_stream << fmt::format("\033[1;{}m", (uint32_t)color);
        } else if (i == end) {
            color_stream << "\033[0m";
        }
        color_stream << text[i];
    }
    return color_stream.str();
}

std::string colorizer::colorize(const std::string &text, term_colors_t color)
{
    return fmt::format("\033[1;{}m{}\033[0m", (uint32_t)color, text);
}
}