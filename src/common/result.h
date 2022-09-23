//
// Created by 12132 on 2022/3/26.
//

#ifndef COMMON_RESULT_H_
#define COMMON_RESULT_H_
#include "result_message.h"
#include <string>

#if _MSC_VER
#if !defined(API_EXPORT)
		#define API_EXPORT __declspec(dllexport)
	#endif
#else
#define API_EXPORT
#endif

namespace gfx {
class result{
public:
	result() = default;
	inline void fail() { succeed_ = false;}
	inline void succeed() { succeed_ = true;}
	inline void add_message(const std::string& code, const std::string& message)
	{
		messages_.emplace_back(code, message, std::string(), result_message::types::info);
	}

	inline void add_message(const std::string& code, const std::string& message, bool error)
	{
		messages_.emplace_back(code, message, std::string(),
	   	error ?  result_message::types::error :  result_message::types::info);
		if (error) {
			fail();
		}
	}

	inline void add_message(const std::string& code, const std::string& message, const std::string& details, bool error)
	{
		messages_.emplace_back(code, message, details, 	error ?  result_message::types::error :  result_message::types::info);
	}

	inline bool is_failed() const
	{
		return !succeed_;
	}

	inline const result_message_list& messages() const
	{
		return messages_;
	}

	inline bool has_code(const std::string& code) const
	{
		for (const auto& msg : messages_) {
			if (msg.code() == code) {
				return true;
			}
		}
		return false;
	}

	inline const result_message* find_code(const std::string& code) const
	{
		for (const auto & message : messages_) {
			if (message.code() == code) {
				return &message;
			}
		}
		return nullptr;
	}

private:
	bool succeed_ = true;
	result_message_list messages_ {};
};
}
#endif // COMMON_RESULT_H_
