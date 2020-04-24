#include <sdliv.h>

#include <string>
#include <iostream>


void sdliv::log(const char * s)
{
	std::cout << s << std::endl;
}

void sdliv::log(const std::string & s)
{
	std::cout << s << std::endl;
}
