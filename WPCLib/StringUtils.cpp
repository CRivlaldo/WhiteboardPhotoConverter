#include "Prerequisites.h"

using namespace WPC;

const std::string StringUtils::ConvertWStringToString(const std::wstring& wString)
{
	const std::locale locale("");
	typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
	const converter_type& converter = std::use_facet<converter_type>(locale);
	std::vector<char> to(wString.length() * converter.max_length());
	std::mbstate_t state;
	const wchar_t* from_next;
	char* to_next;
	const converter_type::result result = converter.out(state, wString.data(), 
		wString.data() + wString.length(), from_next, &to[0], &to[0] + to.size(), to_next);
	if (result == converter_type::ok || result == converter_type::noconv) {
		return std::string(&to[0], to_next);
	}

	return NULL;
}
		
const std::wstring StringUtils::ConvertStringToWString(const std::string& string)
{
	const std::locale locale("");
	typedef std::codecvt<char, wchar_t, std::mbstate_t> converter_type;
	const converter_type& converter = std::use_facet<converter_type>(locale);
	std::vector<wchar_t> to(string.length() * converter.max_length());
	std::mbstate_t state;
	const char* from_next;
	wchar_t* to_next;
	const converter_type::result result = converter.out(state, string.data(), 
		string.data() + string.length(), from_next, &to[0], &to[0] + to.size(), to_next);
	if (result == converter_type::ok || result == converter_type::noconv) {
		return std::wstring(&to[0], to_next);
	}

	return NULL;
}

const std::string StringUtils::ChangeExtension(const std::string& path, const std::string& extensionWithDot)
{
	int dotIndex = path.find_last_of('.');
	if(dotIndex < 0)
		dotIndex = path.length();

	return path.substr(0, dotIndex) + extensionWithDot;
}

const std::string StringUtils::GetExtension(const std::string& path)
{
	int dotIndex = path.find_last_of('.');
	if(dotIndex < 0)
		return "";
	
	std::string extension = path.substr(dotIndex, path.length() - dotIndex);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	return extension;
}

std::string StringUtils::Replace(const std::string& inputString, const std::string& oldValue, 
			const std::string& newValue)
{
	std::string outString = inputString;
	
	size_t startPosition = outString.find(oldValue, 0);
	while(startPosition != std::string::npos)
	{
         outString.replace(startPosition, oldValue.length(), newValue);
		 startPosition = outString.find(oldValue, startPosition + newValue.length());
	}

	return outString;
}