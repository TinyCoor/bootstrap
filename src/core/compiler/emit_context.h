//
// Created by 12132 on 2022/7/23.
//

#ifndef COMPILER_EMIT_CONTEXT_H_
#define COMPILER_EMIT_CONTEXT_H_
#include <string>

namespace gfx::compiler {
enum class emit_context_type_t {
    empty,
    if_element,
    procedure_type
};

enum class emit_access_type_t {
    read,
    write
};

struct procedure_type_data_t {
    std::string identifier_name;
};

struct if_data_t {
    std::string true_branch_label;
    std::string false_branch_label;
};

struct emit_context_t {
    inline static emit_context_t for_if_element(const emit_context_t& parent_context,
        const std::string& true_branch_label, const std::string& false_branch_label)
    {
        auto data = new if_data_t();
        data->true_branch_label = true_branch_label;
        data->false_branch_label = false_branch_label;
        return emit_context_t {
            .type = emit_context_type_t::if_element,
            .data = {
                .if_data = data
            }
        };
    }

    inline static emit_context_t for_procedure_type(const emit_context_t& parent_context,
        const std::string& name) {
        auto data = new procedure_type_data_t();
        data->identifier_name = name;
        return emit_context_t {
            .type = emit_context_type_t::procedure_type,
            .data = {
                .procedure_type = data
            }
        };
    }

    inline static emit_context_t for_read(const emit_context_t& parent_context)
    {
        return emit_context_t {
            .access_type = emit_access_type_t::read,
            .type = parent_context.type,
        };
    }

    inline static emit_context_t for_write(const emit_context_t& parent_context)
    {
        return emit_context_t {
            .access_type = emit_access_type_t::write,
            .type = parent_context.type,
        };
    }

    ~emit_context_t() {
        switch (type) {
            case emit_context_type_t::if_element:
                delete data.if_data;
                break;
            case emit_context_type_t::procedure_type:
                delete data.procedure_type;
                break;
            default:break;
        }
    }

    emit_access_type_t access_type = emit_access_type_t::read;
    emit_context_type_t type = emit_context_type_t::empty;
    union {
        if_data_t* if_data;
        procedure_type_data_t* procedure_type;
    } data{};
};
}

#endif // COMPILER_EMIT_CONTEXT_H_
