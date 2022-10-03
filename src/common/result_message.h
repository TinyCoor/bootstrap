//
// Created by 12132 on 2022/3/26.
//

#ifndef COMMON_RESULT_MESSAGE_H_
#define COMMON_RESULT_MESSAGE_H_
#include <vector>
#include <string>

namespace gfx {

class result_message {
public:
	enum types {
		info,
		error,
		data,
	};

	result_message(const std::string& code, const std::string& message, const std::string& details = "",
		types type = types::info)
		: type_(type), code_(code), message_(message), details_(details)
	{}

	result_message(std::string&& code, std::string&& message, std::string&& details = "",
		types type = types::info)
		: type_(type), code_(std::move(code)), message_(std::move(message)), details_(std::move(details))
	{}

	[[nodiscard]] inline types type() const
	{
		return type_;
	}

	[[nodiscard]] inline bool is_error() const
	{
		return type_== types::error;
	}

	[[nodiscard]] inline const std::string& code() const
	{
		return code_;
	}

	[[nodiscard]] inline const std::string& details() const
	{
		return details_;
	}

	[[nodiscard]] inline const std::string& message() const
	{
		return message_;
	}

private:
	types type_;
	std::string code_{};
	std::string message_{};
	std::string details_{};
};

using result_message_list = std::vector<result_message> ;

}

#endif // COMMON_RESULT_MESSAGE_H_
