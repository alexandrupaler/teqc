#ifndef TRIM_H__
#define TRIM_H__

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <string>

std::string toString (int a);

// trim from start
std::string &ltrim(std::string &s);

// trim from end
std::string &rtrim(std::string &s);

// trim from both ends
std::string &trim(std::string &s);

#endif
