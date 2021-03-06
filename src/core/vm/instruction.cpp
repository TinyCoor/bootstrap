//
// Created by 12132 on 2022/4/3.
//
#include "instruction.h"
#include "common/bytes.h"
#include "fmt/format.h"
#include <iomanip>
namespace gfx {
size_t instruction_t::encode(result& r, uint8_t* heap, uint64_t address)
{
	if (address % alignment != 0) {
		r.add_message("B003", fmt::format("instruction alignment violation: alignment = {} bytes, address = ${:016X}",
			 alignment, address), true);
		return 0;
	}
	uint8_t encoding_size = base_size;
	size_t offset = base_size;

	auto encoding_ptr = heap + address;
	*(encoding_ptr + 1) = static_cast<uint8_t>(op);

	uint8_t size_type_and_operand_count = 0;
	size_type_and_operand_count = set_upper_nybble(size_type_and_operand_count, static_cast<uint8_t>(size));
	size_type_and_operand_count = set_lower_nybble(size_type_and_operand_count, operands_count);
	*(encoding_ptr + 2) = size_type_and_operand_count;

	for (size_t i = 0u; i < operands_count ; ++i) {
		*(encoding_ptr + offset) = static_cast<uint8_t>(operands[i].type);
		++offset;
		++encoding_size;

		if (operands[i].is_reg()) {
			*(encoding_ptr + offset) = operands[i].value.r8;
			++offset;
			++encoding_size;
		} else {
			switch (size) {
				case op_sizes::byte: {
					auto *constant_value_ptr = reinterpret_cast<uint8_t *>(encoding_ptr + offset);
					*constant_value_ptr = static_cast<uint8_t>(operands[i].value.u64);
					offset += sizeof(uint8_t);
					encoding_size += sizeof(uint8_t);
				}break;
				case op_sizes::word: {
					auto *constant_value_ptr = reinterpret_cast<uint16_t *>(encoding_ptr + offset);
					*constant_value_ptr = static_cast<uint16_t>(operands[i].value.u64);
					offset += sizeof(uint16_t);
					encoding_size += sizeof(uint16_t);
					break;
				}
				case op_sizes::dword: {
					if ( operands[i].is_integer()) {
						auto *constant_value_ptr = reinterpret_cast<uint32_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint32_t>(operands[i].value.u64);
						offset += sizeof(uint32_t);
						encoding_size += sizeof(uint32_t);
					} else {
						auto *constant_value_ptr = reinterpret_cast<float *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<float>(operands[i].value.d64);
						offset += sizeof(float);
						encoding_size += sizeof(float);
					}
					break;
				}
				case op_sizes::qword: {
					if (operands[i].is_integer()) {
						auto *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint64_t>(operands[i].value.u64);
						offset += sizeof(uint64_t);
						encoding_size += sizeof(uint64_t);
					} else {
						auto *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<double>(operands[i].value.d64);
						offset += sizeof(double);
						encoding_size += sizeof(double);
					}
					break;
				}
				case op_sizes::none:{
					if (operands[i].is_integer()) {
						r.add_message("B009", "constant integers cannot have a size of 'none'.", true);
					} else {
						r.add_message("B009", "constant floats cannot have a size of 'none', 'byte', or 'word'.", true);
					}
				}break;
			}
		}
	}

	encoding_size = static_cast<uint8_t>(align(encoding_size, alignment));
	*encoding_ptr = encoding_size;

	return encoding_size;
}

size_t instruction_t::decode(result& r, uint8_t* heap, uint64_t address)
{
	if (address % alignment != 0) {
		r.add_message("B003", fmt::format("instruction alignment violation: alignment = {} bytes, address = ${:016X}",
			alignment,address), true);
		return 0;
	}

	uint8_t* encoding_ptr = heap + address;
	uint8_t encoding_size = *encoding_ptr;
	op = static_cast<op_codes>(*(encoding_ptr +1));
	auto op_size_and_operands_count = static_cast<uint8_t>(*(encoding_ptr + 2));
	size = static_cast<op_sizes>(get_upper_nybble(op_size_and_operands_count));
	operands_count = get_lower_nybble(op_size_and_operands_count);

	size_t offset = base_size;
	for (size_t i = 0u; i < operands_count ; ++i) {
		operands[i].type = static_cast<operand_encoding_t::flags_t>(*(encoding_ptr + offset));
		++offset;

		if ((operands[i].is_reg())) {
			operands[i].value.r8 = *(encoding_ptr + offset);
			++offset;
		} else {
			switch (size) {
				case op_sizes::none: {
					if (operands[i].is_integer()) {
						r.add_message("B010","constant integers cannot have a size of 'none'.", true);
					} else {
						r.add_message("B010","constant floats cannot have a size of 'none', 'byte', or 'word'.", true);
					}
					break;
				}
				case op_sizes::byte: {
					auto *constant_value_ptr = reinterpret_cast<uint8_t *>(encoding_ptr + offset);
					operands[i].value.u64 = *constant_value_ptr;
					offset += sizeof(uint8_t);
				}break;
				case op_sizes::word: {
					auto *constant_value_ptr = reinterpret_cast<uint16_t *>(encoding_ptr + offset);
					operands[i].value.u64 = *constant_value_ptr;
					offset += sizeof(uint16_t);
				}break;
				case op_sizes::dword: {
					if (operands[i].is_integer()){
						auto *constant_value_ptr = reinterpret_cast<uint32_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint32_t);
					} else{
						auto *constant_value_ptr = reinterpret_cast<float *>(encoding_ptr + offset);
						operands[i].value.d64 = *constant_value_ptr;
						offset += sizeof(float);
					}
				}break;
				case op_sizes::qword: {
					if (operands[i].is_integer()) {
						auto *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint64_t);
					} else {
						auto *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
						operands[i].value.d64 = *constant_value_ptr;
						offset += sizeof(double);
					}
				}break;
			}
		}
	}

	return encoding_size;
}

size_t instruction_t::align(uint64_t value, size_t size) const
{
	auto offset = value % size;
	return offset ? value + (size - offset) : value;
}

size_t instruction_t::encoding_size() const
{
	size_t encoding_size = base_size;

	for (size_t i = 0; i < operands_count; i++) {
		encoding_size += 1;

		if ((operands[i].is_reg())) {
			encoding_size += sizeof(uint8_t);
		} else {
			switch (size) {
				case op_sizes::none:{
					break;
				}
				case op_sizes::byte:{
					encoding_size += sizeof(uint8_t);
					break;
				}
				case op_sizes::word:{
					encoding_size += sizeof(uint16_t);
					break;
				}
				case op_sizes::dword: {
					if (operands[i].is_integer())  {
						encoding_size += sizeof(uint32_t);
					} else{
						encoding_size += sizeof(float);
					}
				}break;
				case op_sizes::qword: {
					if (operands[i].is_integer()) {
						encoding_size += sizeof(uint64_t);
					} else{
						encoding_size += sizeof(double);
					}
					break;
				}
			}
		}
	}

	encoding_size = static_cast<uint8_t>(align(encoding_size, alignment));

	return encoding_size;
}

void instruction_t::patch_branch_address(uint64_t address, uint8_t index)
{
	operands[index].value.u64 = align(address, alignment);
}

std::string instruction_t::disassemble(const std::function<std::string(uint64_t)> &id_resolver) const
{
    std::stringstream stream;
    auto op_name = op_code_name(op);
    if (!op_name.empty()) {
        std::stringstream mnemonic;
        std::string format_spec;

        mnemonic <<  op_name;
        switch (size) {
            case op_sizes::byte:
                mnemonic << ".B";
                format_spec = "#${:02X}";
                break;
            case op_sizes::word:
                mnemonic << ".W";
                format_spec = "#${:04X}";
                break;
            case op_sizes::dword:
                mnemonic << ".DW";
                format_spec = "#${:08X}";
                break;
            case op_sizes::qword:
                mnemonic << ".QW";
                format_spec = "#${:016X}";
                break;
            default: {
                break;
            }
        }

        stream << std::left << std::setw(10) << mnemonic.str();

        std::stringstream operands_stream;
        for (size_t i = 0; i < operands_count; i++) {
            if (i > 0 && i < operands_count) {
                operands_stream << ", ";
            }

            const auto& operand = operands[i];
            std::string prefix, postfix;

            if (operand.is_negative()) {
                if (operand.is_prefix())
                    prefix = "--";
                else
                    prefix = "-";

                if (operand.is_postfix())
                    postfix = "--";
            } else {
                if (operand.is_prefix())
                    prefix = "++";

                if (operand.is_postfix())
                    postfix = "++";
            }

            if (operand.is_reg()) {
                if (operand.is_integer()) {
                    switch (operand.value.r8) {
                        case i_registers_t::sp: {
                            operands_stream << prefix << "SP" << postfix;
                            break;
                        }
                        case i_registers_t::fp: {
                            operands_stream << prefix << "FP" << postfix;
                            break;
                        }
                        case i_registers_t::pc: {
                            operands_stream << prefix << "PC" << postfix;
                            break;
                        }
                        case i_registers_t::fr: {
                            operands_stream << "FR";
                            break;
                        }
                        case i_registers_t::sr: {
                            operands_stream << "SR";
                            break;
                        }
                        default: {
                            operands_stream << prefix << "I" << std::to_string(operand.value.r8) << postfix;
                            break;
                        }
                    }
                } else {
                    operands_stream << "F" << std::to_string(operand.value.r8);
                }
            } else {
                if (operand.is_integer()) {
                    if (operand.is_unresolved()) {
                        if (id_resolver == nullptr) {
                            operands_stream << fmt::format("id({})", operand.value.u64);
                        }else {
                            operands_stream << id_resolver(operand.value.u64);
                        }
                    } else {
                        if (prefix == "-") {
                            operands_stream << fmt::format("{}", static_cast<int64_t>(operand.value.u64));
                        } else {
                            operands_stream << prefix
                                            << fmt::format(fmt::runtime(format_spec), operand.value.u64)
                                            << postfix;
                        }
                    }
                } else {
                    operands_stream << prefix << fmt::format(fmt::runtime(format_spec), operand.value.d64)
                         << postfix;
                }
            }
        }
        stream << std::left << std::setw(24) << operands_stream.str();
    } else {
        stream << "UNKNOWN";
    }
    return stream.str();
}
}