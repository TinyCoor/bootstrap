//
// Created by 12132 on 2022/9/11.
//
#include "colorizer.h"
#include <fmt/format.h>
#include <sstream>

namespace gfx {
std::string colorizer::colorize_range(const std::string &text, size_t begin, size_t end,
     term_colors_t fg_color, term_colors_t bg_color) noexcept
{
    std::stringstream color_stream;
    for (size_t i = 0; i < text.length() ; ++i) {
        if (i == begin && begin == end) {
            color_stream << fmt::format("{}{}", color_code(make_bg_color(bg_color)), color_code(fg_color));
            color_stream << text[i];
            color_stream << color_code_reset();
        } else {
            if (i == begin) {
                color_stream << fmt::format("{}{}", color_code(make_bg_color(bg_color)), color_code(fg_color));
            } else if (i == end){
                color_stream << color_code_reset();
            }
            color_stream << text[i];
        }
    }
    return color_stream.str();
}

std::string colorizer::colorize(const std::string &text, term_colors_t fg_color, term_colors_t bg_color)
{
    return fmt::format("{}{}{}{}", color_code(make_bg_color(bg_color)), color_code(fg_color), text, color_code_reset());
}

std::string colorizer::color_code(term_colors_t color)
{
    return fmt::format("\033[1;{}m", (uint32_t)color);
}

constexpr const char *colorizer::color_code_reset()
{
    return "\033[0m";
}

constexpr term_colors_t colorizer::make_bg_color(term_colors_t color)
{
    return static_cast<term_colors_t>((uint32_t)color + 10);
}

}