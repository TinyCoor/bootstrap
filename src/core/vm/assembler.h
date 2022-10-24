//
// Created by 12132 on 2022/4/20.
//

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include <stack>
#include "terp.h"
#include "symbol.h"
#include "segment.h"
#include "common/result.h"
#include "common/id_pool.h"
#include "common/source_file.h"
#include "instruction_block.h"
#include "assembly_listing.h"
#include <variant>
namespace gfx {
using directive_param_variant_t = std::variant<std::string, uint64_t>;
enum class directive_type_t : uint8_t {
    section,
    align,
    db,
    dw,
    dd,
    dq,
    rb,
    rw,
    rd,
    rq
};
struct directive_param_t {
    enum flags : uint8_t {
        none        = 0b00000000,
        string      = 0b00000001,
        number      = 0b00000010,
        repeating   = 0b10000000,
    };

    bool is_number() const {
        return (type & flags::number) != 0;
    }

    bool is_string() const {
        return (type & flags::string) != 0;
    }

    bool is_repeating() const {
        return (type & flags::repeating) != 0;
    }

    uint8_t type = flags::none;
    bool required = false;
};

struct directive_t {
    op_sizes size;
    directive_type_t type;
    std::vector<directive_param_t> params {};
    size_t required_operand_count() const {
        size_t count = 0;
        for (const auto& op : params) {
            count += op.required ? 1 : 0;
        }
        return count;
    }
};

inline static std::map<std::string, directive_t> s_directives = {
    {
        "SECTION",
        directive_t{
            op_sizes::none,
            directive_type_t::section,
            {
                {directive_param_t::flags::string, true},
            }
        }
    },
    {
        "ALIGN",
        directive_t{
            op_sizes::byte,
            directive_type_t::align,
            {
                {directive_param_t::flags::number, true},
            }
        }
    },
    {
        "DB",
        directive_t{
            op_sizes::byte,
            directive_type_t::db,
            {
                {directive_param_t::flags::number | directive_param_t::flags::repeating, true},
            }
        }
    },
    {
        "DW",
        directive_t{
            op_sizes::word,
            directive_type_t::dw,
            {
                {directive_param_t::flags::number | directive_param_t::flags::repeating, true},
            }
        }
    },
    {
        "DD",
        directive_t{
            op_sizes::dword,
            directive_type_t::dd,
            {
                {directive_param_t::flags::number | directive_param_t::flags::repeating, true},
            }
        }
    },
    {
        "DQ",
        directive_t{
            op_sizes::qword,
            directive_type_t::dq,
            {
                {directive_param_t::flags::number | directive_param_t::flags::repeating, true},
            }
        }
    },
    {
        "RB",
        directive_t{
            op_sizes::byte,
            directive_type_t::rb,
            {
                {directive_param_t::flags::number, true},
            }
        }
    },
    {
        "RW",
        directive_t{
            op_sizes::word,
            directive_type_t::rw,
            {
                {directive_param_t::flags::number, true},
            }
        }
    },
    {
        "RD",
        directive_t{
            op_sizes::dword,
            directive_type_t::rd,
            {
                {directive_param_t::flags::number, true},
            }
        }
    },
    {
        "RQ",
        directive_t{
            op_sizes::qword,
            directive_type_t::rq,
            {
                {directive_param_t::flags::number, true},
            }
        }
    },

};

inline static directive_t* directive(const std::string& code) {
    const auto it = s_directives.find(code);
    if (it != s_directives.end()) {
        return &it->second;
    }
    return nullptr;
}

class assembler {
public:
	explicit assembler(terp* terp);

    assembler(const assembler& other) = delete;

	virtual ~assembler();

    bool initialize(result& r);

    assembly_listing& listing();

    bool allocate_reg(register_t& reg);

    void free_reg(const register_t &reg);

    instruction_block* pop_block();

    instruction_block* root_block();

    segment_list_t segments() const;

    instruction_block* current_block();

    bool in_procedure_scope() const;

    bool resolve_labels(result& r);

    bool apply_addresses(result& r);

    void pop_target_register();

    void push_block(instruction_block* block);

    register_t* current_target_register();

    void push_target_register(const register_t &reg);

    gfx::segment* segment(const std::string& name);

    bool assemble(result& r, instruction_block* block = nullptr);

    bool assemble_from_source(result& r, source_file& source);

	gfx::segment* segment(const std::string &name, segment_type_t type);

    instruction_block* make_basic_block(instruction_block* parent = nullptr);

    instruction_block* make_procedure_block(instruction_block* parent = nullptr);

private:
    void add_new_block(instruction_block* block);

    block_entry_t* current_entry(instruction_block* block);

private:
	terp* terp_ = nullptr;
    uint64_t location_counter_ = 0;
    assembly_listing listing_ {};
    uint32_t procedure_block_count_ = 0;
    std::vector<instruction_block*> blocks_ {};
    std::stack<instruction_block*> block_stack_{};
    std::stack<register_t> target_registers_ {};
    register_allocator_t register_allocator_ {};
	std::unordered_map<std::string, gfx::segment> segments_{};
};
}
#endif // ASSEMBLER_H_
