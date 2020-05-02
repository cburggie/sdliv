#pragma once

//template functions need to be fully declared within header file

#include <iostream>
#include <string>

namespace sdliv {
	extern void log();

	template<typename T, typename... A>
	void log(T const& t, A const&... args)
	{
		std::cout << stringify(t);
		log(args...);
	};

	inline std::string stringify(std::string v) { return v; }
	inline std::string stringify(const char *c) { return std::string(c); }
	inline std::string stringify(int v) { return std::to_string(v); }

	template<typename T>
	std::string stringify(T const &v)
	{
		return std::string("(Unknown Type)");
	}
}
