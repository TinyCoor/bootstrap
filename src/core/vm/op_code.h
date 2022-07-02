//
// Created by 12132 on 2022/4/3.
//

#ifndef OP_CODE_H_
#define OP_CODE_H_
#include <cstdint>
#include <string>
#include <map>
namespace gfx {
enum class op_codes : uint8_t {
	nop = 1, alloc, free, size,
	load, store, swap, move,
	copy, fill, push, pop,
	dup, add, sub, inc,
	dec, mul, div, mod,
	neg, shl, shr, rol,
	ror, and_op, or_op,
	xor_op, not_op, bis,
	bic, test, cmp, bz,
	bnz, tbz, tbnz, bne,
	beq, bg, bge, bl, ble,
	jsr, rts, jmp, swi, trap,
	ffi, meta, exit,
};

inline static std::map<op_codes, std::string_view> s_op_code_names  = {
	{op_codes::nop,    "NOP"},  {op_codes::alloc,  "ALLOC"}, {op_codes::free, "FREE"},
	{op_codes::load,   "LOAD"}, {op_codes::store,  "STORE"}, {op_codes::size, "SIZE"},
	{op_codes::copy,   "COPY"}, {op_codes::fill,   "FILL"},  {op_codes::move,  "MOVE"},
	{op_codes::push,   "PUSH"}, {op_codes::pop,    "POP"},   {op_codes::inc,    "INC"},
	{op_codes::dec,    "DEC"},  {op_codes::add,    "ADD"},   {op_codes::sub,    "SUB"},
	{op_codes::mul,    "MUL"},  {op_codes::div,    "DIV"},   {op_codes::mod,    "MOD"},
	{op_codes::neg,    "NEG"},  {op_codes::shr,    "SHR"},   {op_codes::shl,    "SHL"},
	{op_codes::ror,    "ROR"},  {op_codes::rol,    "ROL"},   {op_codes::and_op, "AND"},
	{op_codes::or_op,  "OR"},   {op_codes::xor_op, "XOR"},   {op_codes::not_op, "NOT"},
	{op_codes::bis,    "BIS"},  {op_codes::bic,    "BIC"},   {op_codes::test,   "TEST"},
	{op_codes::cmp,    "CMP"},  {op_codes::bz,     "BZ"},    {op_codes::bnz,    "BNZ"},
	{op_codes::tbz,    "TBZ"},  {op_codes::tbnz,   "TBNZ"},  {op_codes::bne,    "BNE"},
	{op_codes::beq,    "BEQ"},  {op_codes::bge,    "BGE"},   {op_codes::bg,     "BG"},
	{op_codes::ble,    "BLE"},  {op_codes::bl,     "BL"},    {op_codes::jsr,    "JSR"},
	{op_codes::dup,    "DUP"},  {op_codes::rts,    "RTS"},   {op_codes::jmp,    "JMP"},
	{op_codes::meta,   "META"}, {op_codes::swi,    "SWI"},   {op_codes::trap,  "TRAP"},
	{op_codes::exit,   "EXIT"}, {op_codes::swap,   "SWAP"},
};
}
#endif // OP_CODE_H_
