#include <sdliv.h>



bool operator<(const std::string & a, const std::string & b)
{
	return (a.compare(b) < 0) ? true : false;
}
