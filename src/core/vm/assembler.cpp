//
// Created by 12132 on 2022/4/20.
//

#include "assembler.h"
#include "fmt/format.h"
#include "common/bytes.h"

namespace gfx {

assembler::assembler(terp* terp)
	: terp_(terp)
{
}

assembler::~assembler()
{
    for (auto &block : blocks_) {
        delete block;
    }
    blocks_.clear();
}

bool assembler::initialize(result &r)
{
    location_counter_ = terp_->heap_vector(heap_vectors_t::program_start);
    return true;
}
bool assembler::assemble(result &r, instruction_block *block)
{
    if (block == nullptr) {
        block = current_block();
    }

    current_block()->walk_blocks([&](instruction_block* block) -> bool {
      for (auto& entry : block->entries()) {
          switch (entry.type()) {
              case block_entry_type_t::instruction: {
                  auto inst = entry.data<instruction_t>();
                  auto inst_size = inst->encode(r, terp_->heap(), entry.address());
                  if (inst_size == 0) {
                      return false;
                  }
                  break;
              }
              case block_entry_type_t::data_definition: {
                  auto data_def = entry.data<data_definition_t>();
                  if (data_def->type == data_definition_type_t::initialized) {
                      auto size_in_bytes = op_size_in_bytes(data_def->size);
                      auto offset = 0;
                      for (auto v : data_def->values) {
                          terp_->write(data_def->size, entry.address() + offset, v);
                          offset += size_in_bytes;
                      }
                  }
              }
              default: {
                  break;
              }
          }
      }
      return true;
    });

    return !r.is_failed();

}

bool assembler::assemble_from_source(result &r, std::istream &source)
{
	return false;
}

gfx::segment *assembler::segment(const std::string &name)
{
	auto it = segments_.find(name);
	if (it == segments_.end()) {
		return nullptr;
	}
	return &it->second;
}

segment_list_t assembler::segments() const
{
    segment_list_t list {};
    for (const auto& it: segments_) {
        list.push_back(it.second);
    }
    return list;
}

instruction_block *assembler::current_block()
{
    if (block_stack_.empty()) {
        return nullptr;
    }
    return block_stack_.top();
}

gfx::segment *assembler::segment(const std::string &name, segment_type_t type)
{
    segments_.insert(std::make_pair(name, gfx::segment(name, type)));
    return segment(name);
}

void assembler::push_block(instruction_block *block)
{
    if (block->type() == instruction_block_type_t::procedure) {
        procedure_block_count_++;
    }
    block_stack_.push(block);
}

instruction_block *assembler::pop_block()
{
    if (block_stack_.empty()) {
        return nullptr;
    }
    auto top = block_stack_.top();
    if (top->type() == instruction_block_type_t::procedure && procedure_block_count_ > 0) {
        procedure_block_count_--;
    }
    block_stack_.pop();
    return top;
}

instruction_block *assembler::make_basic_block(instruction_block* parent)
{
    auto block = new instruction_block(parent ==nullptr ? current_block() : parent,
        instruction_block_type_t::basic);
    add_new_block(block);
    return block;
}

instruction_block *assembler::make_procedure_block(instruction_block* parent)
{
    auto block = new instruction_block(parent == nullptr ? current_block() : parent ,
        instruction_block_type_t::procedure);
    add_new_block(block);
    return block;
}

void assembler::add_new_block(instruction_block *block)
{
    auto source_file = listing_.current_source_file();
    if (source_file != nullptr) {
        block->source_file(source_file);
    }
    blocks_.push_back(block);
    auto top_block = current_block();
    if (top_block != nullptr) {
        top_block->add_block(block);
    }
}
instruction_block *assembler::root_block()
{
    if (blocks_.empty()) {
        return nullptr;
    }
    return blocks_.front();
}

bool assembler::in_procedure_scope() const
{
    return procedure_block_count_ > 0;
}

assembly_listing &assembler::listing()
{
    return listing_;
}

bool assembler::resolve_labels(result &r)
{
    auto root_block = current_block();
    root_block->walk_blocks([&](instruction_block* block) -> bool {
      auto label_refs = block->label_references();
      for (auto label_ref : label_refs) {
          label_ref->resolved = root_block->find_label(label_ref->name);
          if (label_ref->resolved == nullptr) {
              r.add_message("A001", fmt::format("unable to resolve label: {}", label_ref->name), true);
              return false;
          }
      }
      for (auto& entry : block->entries()) {
          if (entry.type() != block_entry_type_t::instruction) {
              continue;
          }

          auto inst = entry.data<instruction_t>();
          for (size_t i = 0; i < inst->operands_count; i++) {
              auto& operand = inst->operands[i];
              if (operand.is_unresolved()) {
                  auto label_ref = block->find_unresolved_label_up(static_cast<uint32_t>(operand.value.u64));
                  if (label_ref != nullptr) {
                      operand.value.u64 = label_ref->resolved->address();
                      operand.clear_unresolved();
                  }
              }
          }
      }

      return true;
    });
    return !r.is_failed();
}

bool assembler::apply_addresses(result &r)
{
    size_t offset = 0;
    current_block()->walk_blocks([&](instruction_block* block) -> bool {
      for (auto& entry : block->entries()) {
          entry.address(location_counter_ + offset);
          switch (entry.type()) {
              case block_entry_type_t::memo: {
                  break;
              }
              case block_entry_type_t::align: {
                  auto alignment = entry.data<align_t>();
                  offset = align(offset, alignment->size);
                  entry.address(location_counter_ + offset);
                  break;
              }
              case block_entry_type_t::section: {
                  auto section = entry.data<section_t>();
                  switch (*section) {
                      case section_t::bss:
                      case section_t::text:
                      case section_t::data:
                      case section_t::ro_data:
                      default: break;
                  }
                  break;
              }
              case block_entry_type_t::instruction: {
                  auto inst = entry.data<instruction_t>();
                  offset += inst->encoding_size();
                  break;
              }
              case block_entry_type_t::data_definition: {
                  auto data_def = entry.data<data_definition_t>();
                  offset += op_size_in_bytes(data_def->size) * data_def->values.size();
                  break;
              }
          }
      }
      return true;
    });
    return !r.is_failed();
}

bool assembler::allocate_reg(i_registers_t& reg)
{
    return i_register_allocator_.allocate(reg);
}

bool assembler::allocate_reg(f_registers_t& reg)
{
    return f_register_allocator_.allocate(reg);
}

void assembler::free_reg(i_registers_t reg)
{
    i_register_allocator_.free(reg);
}

void assembler::free_reg(f_registers_t reg)
{
    f_register_allocator_.free(reg);
}

target_register_t assembler::pop_target_register()
{
    if (target_registers_.empty()) {
        return target_register_t {};
    }
    auto reg = target_registers_.top();
    target_registers_.pop();
    return reg;
}

target_register_t *assembler::current_target_register()
{
    if (target_registers_.empty()) {
        return nullptr;
    }
    return &target_registers_.top();
}

void assembler::push_target_register(i_registers_t reg)
{
    target_register_t target {
        .type = target_register_type_t::integer,
        .reg = {
            .i = reg
        }
    };
    target_registers_.push(target);
}

void assembler::push_target_register(f_registers_t reg)
{
    target_register_t target {
        .type = target_register_type_t::floating_point,
        .reg = {
            .f = reg
        }
    };
    target_registers_.push(target);
}


}