//
// Created by 12132 on 2022/7/31.
//

#ifndef VM_ASSEMBLY_LISTING_H_
#define VM_ASSEMBLY_LISTING_H_

#include <string>
#include <stack>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace gfx {
namespace fs = std::filesystem;
struct listing_source_line_t {
    uint64_t address = 0;
    std::string source {};
};

struct listing_source_file_t {
    void add_source_line(uint64_t address, const std::string& source)
    {
        lines.push_back(listing_source_line_t {
            .address = address,
            .source = source
        });
    }

    void add_blank_lines(uint64_t address, uint16_t count = 1)
    {
        for (uint16_t i = 0; i < count; i++) {
            lines.push_back(listing_source_line_t {
                .address = address,
                .source = ""
            });
        }
    }

    fs::path path;
    std::vector<listing_source_line_t> lines {};
};

class assembly_listing {
public:
    assembly_listing();

    void reset();

    void write(FILE* file);

    listing_source_file_t* current_source_file();

    size_t add_source_file(const fs::path& path);

    void select_source_file(const std::filesystem::path& path);
private:
    listing_source_file_t*  current_source_file_ = nullptr;
    std::unordered_map<std::string, listing_source_file_t> source_files_ {};
};

}
#endif //VM_ASSEMBLY_LISTING_H_
