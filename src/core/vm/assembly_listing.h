//
// Created by 12132 on 2022/7/31.
//

#ifndef VM_ASSEMBLY_LISTING_H_
#define VM_ASSEMBLY_LISTING_H_

#include <string>
#include <vector>

namespace gfx {

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
    void add_blank_lines(uint16_t count = 1) {
        for (uint16_t i = 0; i < count; i++) {
            lines.push_back(listing_source_line_t {
                .address = 0,
                .source = ""
            });
        }
    }
    std::string filename;
    std::vector<listing_source_line_t> lines {};
};

class assembly_listing {
public:
    assembly_listing();

    void reset();

    void write(FILE* file);

    listing_source_file_t* current_source_file();

    void add_source_file(const std::string& filename);

private:
    std::vector<listing_source_file_t> source_files_ {};
};
}
#endif //VM_ASSEMBLY_LISTING_H_
