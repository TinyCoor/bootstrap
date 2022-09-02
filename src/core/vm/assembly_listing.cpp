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
        fmt::print(file, "FILE: {:<64} GFX Compiler\n", source_file.first);
        fmt::print(file, "      {:>91}\n\n", "Assembly Listing (byte code)");
        fmt::print(file, "LINE    ADDRESS     SOURCE\n");
        size_t line_number = 1;
        for (const auto& line : source_file.second.lines) {
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
    return current_source_file_;
}

size_t assembly_listing::add_source_file(const fs::path& path)
{
    source_files_.insert(std::make_pair(path.string(), listing_source_file_t {.path = path}));
}

void assembly_listing::select_source_file(const std::filesystem::path &path)
{
    auto it = source_files_.find(path.string());
    if (it == source_files_.end()) {
        current_source_file_ = nullptr;
        return;
    }
    current_source_file_ = &it->second;
}


}