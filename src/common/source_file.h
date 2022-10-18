//
// Created by 12132 on 2022/9/2.
//

#ifndef COMMON_SOURCE_FILE_H_
#define COMMON_SOURCE_FILE_H_

#include <map>
#include <stack>
#include <string_view>
#include <cstdint>
#include <filesystem>
#include "result.h"
#include "rune.h"
#include "source_location.h"
namespace gfx {
using source_file_range_t = std::pair<size_t, size_t>;
namespace fs = std::filesystem;

struct source_file_range_compare_t {
    bool operator()(const source_file_range_t &lhs, const source_file_range_t &rhs) const
    {
        return lhs.second < rhs.first;
    }
};

struct source_file_line_t {
    size_t end{};
    size_t begin{};
    uint32_t line{};
    uint32_t columns{};
};

class source_file {
public:
    explicit source_file(const fs::path &path);

    ~source_file();

    rune_t next(gfx::result &r);

    [[nodiscard]] size_t pos() const;

    void seek(size_t index);

    void push_mark();

    size_t pop_mark();

    void restore_top_mark();

    [[nodiscard]] bool eof() const;

    [[nodiscard]] bool empty() const;

    [[nodiscard]] size_t length() const;

    [[maybe_unused]] bool load(result &r);

    [[nodiscard]] size_t number_of_lines() const;

    uint8_t operator[](size_t index);

    [[nodiscard]] const fs::path &path() const;

    std::string substring(size_t start, size_t end);

    [[nodiscard]] const source_file_line_t *line_by_number(size_t line) const;

    [[nodiscard]] const source_file_line_t *line_by_index(size_t index) const;

    [[nodiscard]] uint32_t column_by_index(size_t index) const;

    void error(result& r, const std::string& code, const std::string& message, const source_location& location);

private:
    void build_lines(result& r);

private:
    size_t index_ = 0;
    fs::path path_;
    std::vector<uint8_t> buffer_{};
    std::stack<size_t> mark_stack_{};
    std::map<size_t, source_file_line_t *> lines_by_number_{};
    std::map<source_file_range_t, source_file_line_t, source_file_range_compare_t> lines_by_index_range_{};
};
}
#endif // COMMON_SOURCE_FILE_H_
