#include <sdliv.h>

//static members
std::map<std::string,sdliv::FileHandler*> sdliv::FileHandler::tracked_files
		= std::map<std::string,sdliv::FileHandler*>();

sdliv::FileHandler * sdliv::FileHandler::active_image = nullptr;





//static methods
sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const char * filename)
{
	// **FIXME**
	return nullptr;
}






sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const std::string & filename)
{
	// **FIXME**
	return nullptr;
}





int sdliv::FileHandler::track(sdliv::FileHandler * fh)
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::untrack(sdliv::FileHandler * fh)
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::untrack(const char * filename)
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::untrack(const std::string & filename)
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::openDirectory()
{
	// **FIXME**
	return -1;
}





sdliv::Element * sdliv::FileHandler::getActiveImage()
{
	// **FIXME**
	return nullptr;
}





sdliv::Element * sdliv::FileHandler::nextImage()
{
	// **FIXME**
	return nullptr;
}





sdliv::Element * sdliv::FileHandler::prevImage()
{
	// **FIXME**
	return nullptr;
}





//non-static methods
sdliv::FileHandler::FileHandler()
{
	type = FILETYPE_UNSUPPORTED;
	filename = "";
	rwops = nullptr;
	element = nullptr;
	window = nullptr;
}





sdliv::FileHandler::FileHandler(const char * filename)
{
	// **FIXME**
}





sdliv::FileHandler::FileHandler(const std::string & filename)
{
	// **FIXME**
}





sdliv::FileHandler::FileHandler(const sdliv::FileHandler & fh)
{
	// **FIXME**
}





sdliv::FileHandler::~FileHandler()
{
	// **FIXME**
}





sdliv::ImageFileType sdliv::FileHandler::detectImageType()
{
	// **FIXME**
	return FILETYPE_UNSUPPORTED;
}





int sdliv::FileHandler::open()
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::read()
{
	// **FIXME**
	return -1;
}





int sdliv::FileHandler::close()
{
	// **FIXME**
	return -1;
}





