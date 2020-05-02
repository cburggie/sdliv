#ifndef _SDLIV_LOG_H_
#define _SDLIV_LOG_H_

//template functions need to be fully declared within header file

#include <iostream>
#include <string>

namespace sdliv {

	extern void log();

	inline std::string stringify(std::string v) { return v; }
	inline std::string stringify(const char *c) { return std::string(c); }
	inline std::string stringify(int v) { return std::to_string(v); }

	template<typename T>
	std::string stringify(T const &v)
	{
		return "(Cannot print type:" + (std::string)typeid(T).name() + ")";
	}

	template<typename T, typename... A>
	void log(T const& t, A const&... args)
	{
		std::cout << stringify(t) << ' ';
		log(args...);
	};
}

#endif
