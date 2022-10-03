//
// Created by 12132 on 2022/9/2.
//

#include "source_file.h"
#include "fmt/format.h"
#include "common/colorizer.h"
#include <fstream>
namespace gfx {

source_file::source_file(const fs::path &path)
   : path_(path)
{

}
source_file::~source_file() = default;

rune_t source_file::next(result &r)
{
    if (index_ > buffer_.size() - 1) {
        return rune_eof;
    }
    size_t width = 1;
    auto ch = buffer_[index_];
    rune_t rune = ch;
    if (ch == 0) {
        r.add_message("S003", "illegal character NUL", true);
        return rune_invalid;
    } else if (ch >= 0x80) {
        auto cp = utf8_decode((char*)(buffer_.data() + index_), buffer_.size() - index_);
        width = cp.width;
        rune = cp.value;
        if (rune == rune_invalid && width == 1) {
            r.add_message("S001", "illegal utf-8 encoding", true);
            return rune_invalid;
        } else if (rune == rune_bom && index_ > 0) {
            r.add_message("S002", "illegal byte order mark", true);
            return rune_invalid;
        }
    }
    index_ += width;
    return rune;
}

[[maybe_unused]] bool source_file::eof() const
{
    return index_ > buffer_.size() - 1u;
}

bool source_file::empty() const
{
    return buffer_.empty();
}

size_t source_file::length() const
{
    return buffer_.size();
}

[[maybe_unused]] bool source_file::load(result &r)
{
    buffer_.clear();
    lines_by_number_.clear();
    lines_by_index_range_.clear();

    std::ifstream file(path_, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.unsetf(std::ios::skipws);
        file.seekg(0, std::ios::end);
        auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        buffer_.reserve(static_cast<size_t>(file_size));
        buffer_.insert(buffer_.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
        build_lines(r);
    } else {
        r.add_message("S001", fmt::format("unable to open source file: {}", path_.string()), true);
    }
    return !r.is_failed();
}

size_t source_file::number_of_lines() const
{
    return lines_by_number_.size();
}

uint8_t source_file::operator[](size_t index)
{
    return buffer_[index];
}

const fs::path &source_file::path() const
{
    return path_;
}

std::string source_file::substring(size_t start, size_t end)
{
    std::string value;
    auto length = end - start;
    value.reserve(length);
    value.assign((const char*)buffer_.data(), start, length);
    return value;
}

const source_file_line_t *source_file::line_by_number(size_t line) const
{
    auto it = lines_by_number_.find(line);
    if (it == lines_by_number_.end()) {
        return nullptr;
    }
    return it->second;
}

const source_file_line_t *source_file::line_by_index(size_t index) const
{
    auto it = lines_by_index_range_.find(std::make_pair(index, index));
    if (it == lines_by_index_range_.end()) {
        return nullptr;
    }
    return &it->second;
}

void source_file::build_lines(result &r)
{
    uint32_t line = 0;
    uint32_t columns = 0;
    size_t line_start = 0;

    while (true) {
        auto rune = next(r);
        if (rune == rune_invalid) {
            break;
        } else if (rune == rune_bom) {
            rune = next(r);
        }

        const auto end_of_buffer = rune == rune_eof;
        const auto unix_new_line = rune == '\n';
        if (unix_new_line || end_of_buffer) {
            auto end = end_of_buffer ? buffer_.size() : index_ - 1;
            auto it = lines_by_index_range_.insert(std::make_pair(std::make_pair(line_start, end),
                                                                  source_file_line_t {
                                                                      .end = end,
                                                                      .begin = line_start,
                                                                      .line = line,
                                                                      .columns = columns
                                                                  }));
            lines_by_number_.insert(std::make_pair(line, &it.first->second));
            line_start = index_;
            line++;
            columns = 0;
        } else {
            columns++;
        }
        if (rune == rune_eof) {
            break;
        }
    }
    seek(0);
}

void source_file::seek(size_t index)
{
    index_ = index;
}

void source_file::push_mark()
{
    mark_stack_.push(index_);
}

size_t source_file::pop_mark()
{
    if (mark_stack_.empty()) {
        return index_;
    }
    auto mark = mark_stack_.top();
    mark_stack_.pop();
    return mark;
}

uint32_t source_file::column_by_index(size_t index) const
{
    auto line = line_by_index(index);
    if (line == nullptr) {
        return 0;
    }
    return static_cast<uint32_t>(index - line->begin);
}

size_t source_file::pos() const
{
    return index_;
}

void source_file::restore_top_mark()
{
    if (mark_stack_.empty()) {
        return;
    }
    index_ = mark_stack_.top();
}

void source_file::error(result &r, const std::string &code, const std::string &message,
                        const source_location& location)
{
    std::stringstream stream;
    auto number_lines = static_cast<int32_t>(number_of_lines());
    auto start_line = static_cast<int32_t>(location.start().line - 4);
    start_line = start_line < 0 ? 0 : start_line;
    auto stop_line = static_cast<int32_t>(location.end().line + 4);
    stop_line = stop_line >= static_cast<int32_t>(lines_by_number_.size()) ? number_lines - 1 : stop_line;
    auto message_indicator = colorizer::colorize("^ " + message, term_colors_t::red);
    auto target_line = static_cast<int32_t>(location.start().line);
    for (int32_t i = start_line; i < stop_line; i++) {
        auto source_line = line_by_number(i);
        if (source_line == nullptr) {
            break;
        }
        auto source_text = substring(source_line->begin, source_line->end);
        if (i == target_line) {
            stream << fmt::format("{:04d}: ", i + 1)
                   << colorizer::colorize_range(source_text, location.start().column, location.end().column, term_colors_t::magenta) << "\n"
                   << fmt::format("{}{}", std::string(6 + location.start().column, ' '), message_indicator);
        } else {
            stream << fmt::format("{:04d}: ", i + 1)
                   << source_text;
        }

        if (i < static_cast<int32_t>(stop_line - 1)) {
            stream << "\n";
        }
    }

    r.add_message(code, fmt::format("({}@{}:{}){}",path_.filename().string(),
        location.start().line + 1, location.start().column + 1, message), stream.str(), true);
}

}