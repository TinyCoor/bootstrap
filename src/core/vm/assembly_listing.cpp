//
// Created by 12132 on 2022/7/31.
//

#include "assembly_listing.h"
#include <fmt/format.h>
namespace gfx {
assembly_listing::assembly_listing() = default;

void assembly_listing::reset()
{
    source_files_.clear();
}

void assembly_listing::write(FILE* file)
{
    size_t count = 0;
    for (const auto& source_file : source_files_) {
        fmt::print(file, "FILE: {:<64} GFX Compiler\n", source_file.filename);
        fmt::print(file, "      {:>91}\n\n", "Assembly Listing (byte code)");
        fmt::print(file, "LINE    ADDRESS     SOURCE\n");
        size_t line_number = 1;
        for (const auto& line : source_file.lines) {
            fmt::print(file, "{:06d}: ${:08x}   {}\n", line_number++, line.address, line.source);
        }
        count++;
        if (count < source_files_.size()) {
            fmt::print(file, "\n\n");
        }
    }
}

listing_source_file_t* assembly_listing::current_source_file()
{
    if (source_file_stack.empty()) {
        return nullptr;
    }
    return &source_files_[source_file_stack.top()];
}

size_t assembly_listing::add_source_file(const std::string& filename)
{
    if (source_files_.capacity() < 256) {
        source_files_.reserve(256);
    }
    source_files_.push_back(listing_source_file_t {.filename = filename});
    return source_files_.size() - 1u;
}

void assembly_listing::push_source_file(size_t index)
{
    source_file_stack.push(index);
}

void assembly_listing::pop_source_file()
{
    if (source_file_stack.empty()) {
        return;
    }
    source_file_stack.pop();
}

}