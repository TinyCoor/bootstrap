//
// Created by 12132 on 2022/7/31.
//

#ifndef VM_ASSEMBLY_LISTING_H_
#define VM_ASSEMBLY_LISTING_H_

#include <string>
#include <stack>
#include <vector>

namespace gfx {

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

    struct listing_source_line_t {
        uint64_t address = 0;
        std::string source {};
    };
    std::string filename;
    std::vector<listing_source_line_t> lines {};
};

class assembly_listing {
public:
    assembly_listing();

    void reset();

    void write(FILE* file);

    void push_source_file(size_t index);

    void pop_source_file();

    listing_source_file_t* current_source_file();

    size_t add_source_file(const std::string& filename);

private:
    std::stack<size_t> source_file_stack{};
    std::vector<listing_source_file_t> source_files_ {};
};
}
#endif //VM_ASSEMBLY_LISTING_H_
