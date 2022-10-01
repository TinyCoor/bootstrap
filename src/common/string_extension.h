//
// Created by 12132 on 2022/9/30.
//

#ifndef COMMON_STRING_EXTENSION_H_
#define COMMON_STRING_EXTENSION_H_
#include <string>
#include <vector>
namespace gfx {

static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

static inline void to_lower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

static inline void to_upper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

// trim from start (copying)
static inline std::string ltrimmed(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimmed(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimmed(std::string s)
{
    trim(s);
    return s;
}

std::string word_wrap(std::string text, size_t width, size_t right_pad = 0, const char& fill = ' ');

std::pair<std::string, std::string> size_to_units(size_t size);

std::string list_to_string(const std::vector<std::string>& list, const char& sep = ',');

std::vector<std::string> string_to_list(const std::string& value, const char& sep = ',');

}

#endif // COMMON_STRING_EXTENSION_H_
