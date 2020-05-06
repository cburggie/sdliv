#ifndef _SDLIV_LOG_H_
#define _SDLIV_LOG_H_

//template functions need to be fully declared within header file

#include <iostream>
#include <string>

// Expand call to include filename and line number
#define log(...) _log(__FILE__, __LINE__, __VA_ARGS__)

namespace sdliv {
	// privatize stringify(...) within this file
	namespace {
		// end of recursive variadic call
		inline ::std::string stringify(void) { return ::std::string(); }

		// individual types that we know how to stringify
		inline ::std::string stringify(::std::string v) { return v; }
		inline ::std::string stringify(const char *c) { return ::std::string(c); }
		inline ::std::string stringify(int v) { return ::std::to_string(v); }
		inline ::std::string stringify(bool v) { return ::std::string(v ? "true" : "false"); }

		// fallback for unknown type, print the type name
		template<typename T>
		inline ::std::string stringify(T const &v)
		{
			return "(Cannot stringify \"" + (::std::string)typeid(T).name() + "\")";
		}

		// recursive variadic call
		template<typename T, typename... A>
		inline ::std::string stringify(T const& t, A const&... args)
		{
			return stringify(t) + " " + stringify(args...);
		}
	}
	// basic log function
	template<typename... A>
	inline void _log(char const* file, int const line, A const&... args)
	{
		std::cout << std::string(file) << ": " << line << " - " << stringify(args...) << std::endl;
	};
}

#endif
