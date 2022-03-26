//
// Created by 12132 on 2022/3/26.
//

#ifndef BOOTSTRAP__RESULT_MESSAGE_H_
#define BOOTSTRAP__RESULT_MESSAGE_H_
#include <vector>
#include <string>

namespace cfg{

class result_message{
public:
	enum types{
		info,
		error,
		data,
	};

	result_message(const std::string& code, const std::string& message, const std::string& details="",
		types type = types::info)
		: type_(type), code_(code), message_(message), details_(details)
	{}

	inline types type() const {return type_;}
	inline bool is_error() const {return type_== types::error;}
	inline const std::string& code() const {return code_;}
	inline const std::string& details() const {return details_;}
	inline const std::string& message() const {return message_;}

private:
	types type_;
	std::string code_{};
	std::string message_{};
	std::string details_{};
};

using result_message_list =  std::vector<result_message> ;

}

#endif //BOOTSTRAP__RESULT_MESSAGE_H_
