//
// Created by 12132 on 2022/9/2.
//

#include "source_file.h"
#include "fmt/format.h"
#include <fstream>
namespace gfx {

source_file::source_file(const fs::path &path)
   : path_(path)
{

}
source_file::~source_file()
{

}
rune_t source_file::next()
{
    if (index_ >= buffer_.size()) {
        return rune_eof;
    }
    rune_t c = buffer_[index_];
    index_++;
    return c;
}

bool source_file::eof() const
{
    return index_ >= buffer_.size();
}

bool source_file::empty() const
{
    return buffer_.empty();
}

size_t source_file::length() const
{
    return buffer_.size();
}

bool source_file::load(result &r)
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
        buffer_.insert(buffer_.begin(),
            std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
        build_lines();
    } else {
        r.add_message("S001", fmt::format("unable to open source file: {}", path_.string()),
            true );
    }
    return true;
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

void source_file::build_lines()
{
    uint32_t line = 0;
    uint32_t columns = 0;
    size_t line_start = 0;

    for (size_t i = 0; i < buffer_.size(); i++) {
        auto end_of_buffer = i == buffer_.size() - 1;
        if (buffer_[i] == '\n' || end_of_buffer) {
            auto end = end_of_buffer ? buffer_.size() : i;
            auto it = lines_by_index_range_.insert(std::make_pair(
                std::make_pair(line_start, end),
                source_file_line_t {
                    .end = end,
                    .begin = line_start,
                    .line = line,
                    .columns = columns
                }));
            lines_by_number_.insert(std::make_pair(line, &it.first->second));
            line_start = i + 1;
            line++;
            columns = 0;
        } else {
            columns++;
        }
    }
}
}