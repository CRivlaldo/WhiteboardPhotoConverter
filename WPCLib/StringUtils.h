#pragma once

namespace WPC
{
	class StringUtils
	{
	public:
		static const std::string ConvertWStringToString(const std::wstring& wString);
		static const std::wstring ConvertStringToWString(const std::string& string);
		static const std::string ChangeExtension(const std::string& path, 
			const std::string& extensionWithDot);
		static const std::string GetExtension(const std::string& path);
		static std::string Replace(const std::string& inputString, const std::string& oldValue, 
			const std::string& newValue);

		static inline std::string &LeftTrim(std::string &s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
					std::not1(std::ptr_fun<int, int>(std::isspace))));

				return s;
		}

		static inline std::string &RightTrim(std::string &s) {
				s.erase(std::find_if(s.rbegin(), s.rend(), 
					std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());

				return s;
		}

		static inline std::string &Trim(std::string &s) {
				return LeftTrim(RightTrim(s));
		}
	};
}
