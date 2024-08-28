#pragma once
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <Windows.h>
#include <boost/format.hpp>



namespace Utils {

	// convert string to wstring
	/*static std::wstring wstring_cast(const std::string& str, const std::locale& loc = std::locale())
	{
		std::vector<wchar_t> buf(str.size());
		std::use_facet<std::ctype<wchar_t>>(loc).widen(str.data(), 
			str.data() + str.size(), buf.data());

		return std::wstring(buf.data(), buf.size());
	}*/

	// convert wstring to string with '?' as default character
	static std::string string_cast(const std::wstring& wstr, const std::locale& loc = std::locale())
	{
		/*std::vector<char> buf(str.size());
		std::use_facet<std::ctype<wchar_t>>(loc).narrow(str.data(),
			str.data() + str.size(), '?', buf.data());

		return std::string(buf.data(), buf.size());*/
		int len = WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), wstr.length() + 1, 0, 0, 0, FALSE);
		char* buf = new char[len];
		WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), wstr.length() + 1, buf, len, 0, FALSE);
		std::string str(buf);
		delete[] buf;
		return str;
	}

	static std::wstring wstring_cast(const std::string& str)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length() + 1, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length() + 1, buf, len);
		std::wstring wstr(buf);
		delete[] buf;
		return wstr;
	}

	// convert utf-8 string to wstring
	static std::wstring utf8_to_wstring(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.from_bytes(str);
	}

	// convert wstring to utf-8 string
	static std::string wstring_to_utf8(const std::wstring& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(str);
	}

	static std::string int_to_string(int n)
	{
		boost::format fmt = boost::format("%d") % n;
		return fmt.str();
	}

	static std::wstring int_to_wstring(int n)
	{
		boost::format fmt = boost::format("%d") % n;
		return wstring_cast(fmt.str());
	}


#define LPCTSTR_CAST(str) wstring_cast(str).c_str()
#define INT_TO_LPCTSTR(num) int_to_wstring(num).c_str()
#define STRING_CAST(str) string_cast(str)

} // namespace Utils

