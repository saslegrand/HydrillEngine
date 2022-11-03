#include "Tools/StringHelper.hpp"

#include <algorithm>
#include <cctype>

std::string StringHelper::ToUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::toupper(c); }
	);
	return str;
}

bool StringHelper::StrStrCaseUnsensitive(const std::string& str1, const std::string& str2)
{
	return ToUpper(str1).find(ToUpper(str2)) != std::string::npos;
}