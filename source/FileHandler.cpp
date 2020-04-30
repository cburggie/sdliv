#include <sdliv.h>
#include <vector>





//static members
std::map<std::string,sdliv::FileHandler*> sdliv::FileHandler::tracked_files
		= std::map<std::string,sdliv::FileHandler*>();

sdliv::FileHandler * sdliv::FileHandler::active_image = nullptr;





//static methods
sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const char * filename)
{
	FileHandler * fh = new FileHandler(filename);
	if (fh == nullptr)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- failed to allocate FileHandler");
		return nullptr;
	}

	if (fh->type == FILETYPE_UNSUPPORTED)
	{
		delete fh;
		return nullptr;
	}

	return fh;
}






sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const std::string & filename)
{
	return openFileIfSupported(filename.c_str());
}





int sdliv::FileHandler::track(sdliv::FileHandler * fh)
{
	if (tracked_files.count(fh->filename) != 0)
	{
		log("sdliv::FileHandler::track() -- file already tracked");
		return -1;
	}

	tracked_files[fh->filename] = fh;
	return 0;
}





int sdliv::FileHandler::untrack(sdliv::FileHandler * fh)
{
	return untrack(fh->filename);
}





int sdliv::FileHandler::untrack(const char * filename)
{
	if (tracked_files.count(filename) == 0)
	{
		log("sdliv::FileHandler::untrack() -- file not tracked");
		return -1;
	}

	tracked_files.erase(filename);
	return 0;
}





int sdliv::FileHandler::untrack(const std::string & filename)
{
	return untrack(filename.c_str());
}





int sdliv::FileHandler::openDirectory()
{
	int count = 0;

	for (auto& f: std::filesystem::directory_iterator(std::filesystem::current_path()))
	{
		FileHandler * fh = openFileIfSupported(f.path().filename().string());

		if (fh != nullptr)
		{
			track(fh);
			count++;
		}
	}

	return count;
}





sdliv::Element * sdliv::FileHandler::getActiveImage()
{
	if (active_image == nullptr) return nullptr;

	active_image->update();

	return active_image->element;
}





sdliv::Element * sdliv::FileHandler::nextImage()
{
	if (tracked_files.size() == 0) return nullptr;

	if (active_image == nullptr)
	{
		FileHandler * fh = (*tracked_files.begin()).second;
		return (fh == nullptr) ? nullptr : fh->element;
	}

	auto iter = tracked_files.find(active_image->filename);
	++iter;

	if (iter == tracked_files.end())
	{
		iter = tracked_files.begin();
	}

	active_image = iter->second;

	return getActiveImage();
}





sdliv::Element * sdliv::FileHandler::prevImage()
{
	if (tracked_files.size() == 0) return nullptr;

	if (active_image == nullptr)
	{
		return nullptr;
	}

	auto i = tracked_files.find(active_image->filename);

	if (i == tracked_files.begin())
	{
		i = tracked_files.end();
	}

	active_image = (--i)->second;

	return getActiveImage();
}





//non-static methods
sdliv::FileHandler::FileHandler()
{
	type = FILETYPE_UNSUPPORTED;
	filename = "";
	rwops = nullptr;
	element = nullptr;
	window = Window::getFirstWindow();
	fs_entry = std::filesystem::directory_entry();
	timestamp = std::filesystem::file_time_type();
}





sdliv::FileHandler::FileHandler(const char * filename)
{
	type = FILETYPE_UNSUPPORTED;
	this->filename = "";
	rwops = nullptr;
	element = nullptr;
	window = Window::getFirstWindow();
	fs_entry = std::filesystem::directory_entry();
	timestamp = std::filesystem::file_time_type();
	setTarget(filename);
}





sdliv::FileHandler::FileHandler(const std::string & filename)
{
	type = FILETYPE_UNSUPPORTED;
	this->filename = "";
	rwops = nullptr;
	element = nullptr;
	window = Window::getFirstWindow();
	fs_entry = std::filesystem::directory_entry();
	timestamp = std::filesystem::file_time_type();
	setTarget(filename);
}





sdliv::FileHandler::FileHandler(const sdliv::FileHandler & fh)
{
	// **FIXME** this implementation _will_ cause problems with the destructor
	log("sdliv::FileHandler::FileHandler(const sdliv::FileHandler&) -- copy constructor called");

	filename = fh.filename;
	rwops = fh.rwops;
	element = fh.element;
	window = fh.window;
	fs_entry = fh.fs_entry;
	timestamp = fh.timestamp;
}





sdliv::FileHandler::~FileHandler()
{
	//cleanup element
	if (element != nullptr)
	{
		if (window != nullptr)
		{
			window->removeElement(element);
		}

		delete element;
		element = nullptr;
	}

	//cleanup rwops
	if (rwops != nullptr) close();
}





int sdliv::FileHandler::setTarget(const char * fn)
{
	//filename is _not_ the full path
	std::filesystem::path p = std::filesystem::current_path() / fn;
	filename = p.filename().string().c_str();
	fs_entry = std::filesystem::directory_entry(p);
	if (!fs_entry.exists() || !fs_entry.is_regular_file())
	{
		log("sdliv::FileHandler:;setTarget() -- file does not exist", fn);
		filename = "";
	}
	timestamp = fs_entry.last_write_time();
	return 0;
}





int sdliv::FileHandler::setTarget(const std::string & fn)
{
	return setTarget(fn.c_str());
}





sdliv::ImageFileType sdliv::FileHandler::detectImageType()
{
	bool close_when_done = false;
	if (rwops == nullptr)
	{
		if (!open())
		{
			type = FILETYPE_UNSUPPORTED;
			return type;
		}

		close_when_done = true;
	}

	if      (IMG_isJPG(rwops))  { type = FILETYPE_JPG; }
	else if (IMG_isPNG(rwops))  { type = FILETYPE_PNG; }
	else if (IMG_isGIF(rwops))  { type = FILETYPE_GIF; }
	else if (IMG_isWEBP(rwops)) { type = FILETYPE_WEBP; }
	else if (IMG_isTIF(rwops))  { type = FILETYPE_TIF; }
	else if (IMG_isSVG(rwops))  { type = FILETYPE_SVG; }
	else if (IMG_isICO(rwops))  { type = FILETYPE_ICO; }
	else if (IMG_isCUR(rwops))  { type = FILETYPE_CUR; }
	else if (IMG_isBMP(rwops))  { type = FILETYPE_BMP; }
	else if (IMG_isLBM(rwops))  { type = FILETYPE_LBM; }
	else if (IMG_isPCX(rwops))  { type = FILETYPE_PCX; }
	else if (IMG_isPNM(rwops))  { type = FILETYPE_PNM; }
	else if (IMG_isXCF(rwops))  { type = FILETYPE_XCF; }
	else if (IMG_isXPM(rwops))  { type = FILETYPE_XPM; }
	else if (IMG_isXV(rwops))   { type = FILETYPE_XV; }
	else                        { type = FILETYPE_UNSUPPORTED; }

		if (close_when_done) close();
	return type;
}





int sdliv::FileHandler::update()
{
	if (element == nullptr || fs_entry.last_write_time() > timestamp)
	{
		open();
		read();
		close();
		return 1;
	}

	return 0;
}





int sdliv::FileHandler::open()
{
	rwops = SDL_RWFromFile(fs_entry.path().string().c_str(), "rb");
	return (rwops == nullptr) ? -1 : 0;
}





int sdliv::FileHandler::read()
{
	SDL_assert(window != nullptr);

	if (rwops == nullptr)
	{
		log("sdliv::FileHandler::read() -- file not open");
		return -1;
	}

	SDL_Surface * s = nullptr;

	switch (type)
	{
		case FILETYPE_ICO:
			s = IMG_LoadICO_RW(rwops);
			break;
		case FILETYPE_CUR:
			s = IMG_LoadCUR_RW(rwops);
			break;
		case FILETYPE_BMP:
			s = IMG_LoadBMP_RW(rwops);
			break;
		case FILETYPE_GIF:
			s = IMG_LoadGIF_RW(rwops);
			break;
		case FILETYPE_JPG:
			s = IMG_LoadJPG_RW(rwops);
			break;
		case FILETYPE_LBM:
			s = IMG_LoadLBM_RW(rwops);
			break;
		case FILETYPE_PCX:
			s = IMG_LoadPCX_RW(rwops);
			break;
		case FILETYPE_PNG:
			s = IMG_LoadPNG_RW(rwops);
			break;
		case FILETYPE_PNM:
			s = IMG_LoadPNM_RW(rwops);
			break;
		case FILETYPE_SVG:
			s = IMG_LoadSVG_RW(rwops);
			break;
		case FILETYPE_TIF:
			s = IMG_LoadTIF_RW(rwops);
			break;
		case FILETYPE_XCF:
			s = IMG_LoadXCF_RW(rwops);
			break;
		case FILETYPE_XPM:
			s = IMG_LoadXPM_RW(rwops);
			break;
		case FILETYPE_XV:
			s = IMG_LoadXV_RW(rwops);
			break;
		case FILETYPE_WEBP:
			s = IMG_LoadWEBP_RW(rwops);
			break;
		default:
			log("sdliv::FileHandler::read() -- file type unsupported");
			return -1;
	}

	if (element != nullptr)
	{
		if (window != nullptr) window->removeElement(element);
		delete element;
		element = nullptr;
	}

	element = window->createElement();
	element->createFromSurface(s);

	return 0;
}





int sdliv::FileHandler::close()
{
	if (rwops == nullptr)
	{
		log("sdliv::FileHandler::close() -- file not open");
		return -1;
	}

	int error = SDL_RWclose(rwops);
	if (error) log("sdliv::FileHandler::close() -- error on SDL_RWclose()");

	return error;
}





