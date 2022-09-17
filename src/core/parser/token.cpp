//
// Created by 12132 on 2022/4/1.
//

#include "token.h"
#include <cerrno>
#include <climits>
#include <cstdlib>

namespace gfx {

bool token_t::as_bool() const
{
	return value == "true";
}

bool token_t::is_label() const
{
	return type == token_types_t::label;
}

[[maybe_unused]] bool token_t::is_boolean() const
{
	return type == token_types_t::true_literal || type == token_types_t::false_literal;
}

bool token_t::is_line_comment() const
{
	return type == token_types_t::line_comment;
}

bool token_t::is_block_comment() const
{
	return type == token_types_t::block_comment;
}

bool token_t::is_numeric() const
{
	return type == token_types_t::number_literal;
}

std::string token_t::name() const
{
	auto it = s_type_to_name.find(type);
	if (it == s_type_to_name.end()) {
        return "unknown";
    }
	return it->second.data();
}

conversion_result_t token_t::parse(double& out) const {
	const char* s = value.c_str();
	char* end;
	errno = 0;
	out = strtod(s, &end);
	if (errno == ERANGE) {
        return conversion_result_t::overflow;
    }
	if (*s == '\0' || *end != '\0') {
        return conversion_result_t::inconvertible;
    }
	return conversion_result_t::success;
}

conversion_result_t token_t::parse(int64_t& out) const
{
	const char* s = value.data();
	char* end;
	errno = 0;
	out = strtoll(s, &end, radix);
	if ((errno == ERANGE && out == LONG_MAX) || out > UINT_MAX) {
		return conversion_result_t::overflow;
	}
	if ((errno == ERANGE && out == LONG_MIN)) {
		return conversion_result_t::underflow;
	}
	if (*s == '\0' || *end != '\0') {
		return conversion_result_t::inconvertible;
	}
	return conversion_result_t::success;
}

conversion_result_t token_t::parse(uint64_t& out) const
{
	const char* s = value.data();
	char* end;
	errno = 0;
	out = strtoul(s, &end, radix);
	if ((errno == ERANGE && out == LONG_MAX) || out > UINT_MAX) {
		return conversion_result_t::overflow;
	}
	if (*s == '\0' || *end != '\0') {
		return conversion_result_t::inconvertible;
	}
	return conversion_result_t::success;
}
}