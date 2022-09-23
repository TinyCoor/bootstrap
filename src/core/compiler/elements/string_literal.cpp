//
// Created by 12132 on 2022/4/23.
//

#include "string_literal.h"
#include "program.h"
namespace gfx::compiler {
string_literal::string_literal(block* parent, const std::string& value)
	: element(parent, element_type_t::string_literal), value_(value)
{
}

std::string string_literal::value() const
{
	return value_;
}

compiler::type *string_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type(qualified_symbol_t{.name =  "string"});
}

bool string_literal::on_as_string(std::string &value) const
{
     value = value_;
	 return true;
}

bool compiler::string_literal::on_emit(result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->current_block();
    auto target_reg = instruction_block->current_target_register();
    /// TODO FIX BUG
    instruction_block->move_label_to_ireg_with_offset(target_reg->reg.i, label_name(), 4);
    return true;
}

bool string_literal::on_is_constant() const
{
    return true;
}
std::string string_literal::escaped_value() const
{
    std::stringstream stream;
    bool escaped = false;
    for (auto& ch : value_) {
        if (ch == '\\') {
            escaped = true;
        } else {
            if (escaped) {
                if (ch == 'n')
                    stream << "\n";
                else if (ch == 'r')
                    stream << "\r";
                else if (ch == 't')
                    stream << "\t";
                else if (ch == '\\')
                    stream << "\\";
                else if (ch == '0')
                    stream << '\0';

                escaped = false;
            } else {
                stream << ch;
            }
        }
    }
    return stream.str();
}

}