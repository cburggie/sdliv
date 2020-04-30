#include <sdliv.h>

#include <string>
#include <iostream>


void sdliv::log(const char * s, const char * arg)
{
	std::cout << s;
	if (arg != nullptr) std::cout << ": " << arg;
	std::cout << std::endl;
}

void sdliv::log(const std::string & s, const char * arg)
{
	log(s.c_str(), arg);
}
