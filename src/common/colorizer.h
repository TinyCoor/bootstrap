//
// Created by 12132 on 2022/9/11.
//

#ifndef COMMON_COLORIZER_H_
#define COMMON_COLORIZER_H_
#include <cstdint>
#include <string_view>
#include <string>
namespace gfx {
enum class term_colors_t : uint8_t {
    black = 30,
    red,
    green,
    yellow,
    blue,
    magenta,
    white,
};

class colorizer {
public:
    static  std::string color_code(term_colors_t color);

    static constexpr const char *color_code_reset();

    static constexpr term_colors_t make_bg_color(term_colors_t color);

    static std::string colorize_range(const std::string &text,
        size_t begin, size_t end, term_colors_t fg_color, term_colors_t bg_color =term_colors_t::black) noexcept;

    static std::string colorize(const std::string &text, term_colors_t fg_color, term_colors_t bg_color =term_colors_t::black);

private:

};
}
#endif // COMMON_COLORIZER_H_
