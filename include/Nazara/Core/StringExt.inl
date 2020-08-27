// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringExt.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool IsNumber(const char* str)
	{
		std::size_t size = std::strlen(str);
		return IsNumber(std::string_view(str, size));
	}

	bool IsNumber(const std::string_view& str)
	{
		return !str.empty() && std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
	}

	bool MatchPattern(const std::string_view& str, const char* pattern)
	{
		if (!pattern)
			return false;

		return MatchPattern(str, std::string_view(pattern, std::strlen(pattern)));
	}

	template<typename... Args> bool StartsWith(const std::string_view& str, const char* s, Args&&... args)
	{
		std::size_t size = std::strlen(s);
		return StartsWith(str, std::string_view(s, size), std::forward<Args>(args)...);
	}

	bool StartsWith(const std::string_view& str, const std::string_view& s)
	{
		//FIXME: Replace with proper C++20 value once it's available
#if __cplusplus > 201703L
		// C++20
		return str.starts_with(s);
#else
		return str.compare(0, s.size(), s.data()) == 0;
#endif
	}

	template<typename F>
	bool SplitString(const std::string_view& str, const std::string_view& token, F&& func)
	{
		std::size_t pos = 0;
		std::size_t previousPos = 0;
		while ((pos = str.find(token, previousPos)) != std::string::npos)
		{
			std::size_t splitPos = previousPos;
			previousPos = pos + token.size();

			if (!func(str.substr(splitPos, pos - splitPos)))
				return false;
		}

		if (previousPos < str.size())
			return func(str.substr(previousPos));
		else
			return true;
	}

	template<typename F>
	bool SplitStringAny(const std::string_view& str, const std::string_view& token, F&& func)
	{
		std::size_t pos = 0;
		std::size_t previousPos = 0;
		while ((pos = str.find_first_of(token, previousPos)) != std::string::npos)
		{
			std::size_t splitPos = previousPos;
			previousPos = pos + 1;

			if (!func(str.substr(splitPos, pos - splitPos)))
				return false;
		}

		return func(str.substr(previousPos));
	}

	inline std::string ToLower(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToLower(std::string_view(str, size));
	}

	inline std::string ToLower(const char* str, UnicodeAware)
	{
		std::size_t size = std::strlen(str);
		return ToLower(std::string_view(str, size), UnicodeAware{});
	}

	inline std::string ToUpper(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUpper(std::string_view(str, size));
	}

	inline std::string ToUpper(const char* str, UnicodeAware)
	{
		std::size_t size = std::strlen(str);
		return ToUpper(std::string_view(str, size), UnicodeAware{});
	}

	inline std::u16string ToUtf16String(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUtf16String(std::string_view(str, size));
	}

	inline std::u32string ToUtf32String(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToUtf32String(std::string_view(str, size));
	}

	inline std::wstring ToWideString(const char* str)
	{
		std::size_t size = std::strlen(str);
		return ToWideString(std::string_view(str, size));
	}

}

#include <Nazara/Core/DebugOff.hpp>
