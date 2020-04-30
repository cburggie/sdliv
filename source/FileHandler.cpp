#include <sdliv.h>
#include <vector>


#include <set>

static std::set<std::string> supported_extensions = {
	".ico",
	".cur",
	".bmp",
	".gif",
	".jpg",
	".lbm",
	".pcx",
	".png",
	".pnm",
	".svg",
	".tif",
	".xcf",
	".xpm",
	".xv",
	".webp"
};

static bool has_valid_extension(const std::string & fn)
{
	size_t pos = fn.find_last_of('.');
	return ((pos != std::string::npos) && (supported_extensions.count(fn.substr(pos)) > 0)) ? true : false;
}




//static members
std::map<std::string,sdliv::FileHandler*> sdliv::FileHandler::tracked_files
		= std::map<std::string,sdliv::FileHandler*>();

sdliv::FileHandler * sdliv::FileHandler::active_image = nullptr;





//static methods
sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const char * filename)
{
	if (!has_valid_extension(filename))
	{
		log("sdliv::FileHandler::openFileIfSupported() -- file extension not supported", filename);
		return nullptr;
	}
	FileHandler * fh = new FileHandler(filename);
	if (fh == nullptr)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- failed to allocate FileHandler");
		return nullptr;
	}

	if (fh->type == FILETYPE_UNSUPPORTED)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- unsupported file",filename);
		delete fh;
		return nullptr;
	}

	if (tracked_files.count(fh->filename) > 0)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- file already tracked",fh->filename);
		untrack(fh->filename);
	}

	track(fh);

	return fh;
}






sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const std::string & filename)
{
	return openFileIfSupported(filename.c_str());
}





int sdliv::FileHandler::track(sdliv::FileHandler * fh)
{
	SDL_assert(fh != nullptr);

	if (tracked_files.count(fh->filename) != 0)
	{
		log("sdliv::FileHandler::track() -- file already tracked",fh->filename);
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





int sdliv::FileHandler::untrackAll()
{
	for (auto & p : tracked_files)
	{
		if (p.second != nullptr)
		{
			delete p.second;
		}
	}

	tracked_files.clear();

	return 0;
}





int sdliv::FileHandler::openDirectory()
{
	int count = 0;

	for (auto& f: std::filesystem::directory_iterator(std::filesystem::current_path()))
	{
		FileHandler * fh = openFileIfSupported(f.path().filename().string());

		if (fh != nullptr)
		{
			count++;
		}
	}

	return count;
}





sdliv::Element * sdliv::FileHandler::getActiveImage()
{
	if (active_image == nullptr)
	{
		if (tracked_files.size() > 0)
		{
			active_image = tracked_files.begin()->second;
		}

		else
		{
			return nullptr;
		}
	}


	active_image->update();

	return active_image->element;
}





sdliv::Element * sdliv::FileHandler::nextImage()
{
	if (tracked_files.size() == 0)
	{
		log("sdliv::FileHandler::nextImage() -- not tracking any files");
		return nullptr;
	}

	if (active_image == nullptr)
	{
		active_image = tracked_files.begin()->second;
		return getActiveImage();
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
		active_image = (--(tracked_files.end()))->second;
		return getActiveImage();
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



std::filesystem::path sdliv::FileHandler::parent_path() const
{
	return fs_entry.path().parent_path();
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
	type = detectImageType();
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
		if (open())
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
#ifndef WIN32
	else if (IMG_isSVG(rwops))  { type = FILETYPE_SVG; }
#endif
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
		case FILETYPE_UNSUPPORTED:
			log("sdliv::FileHandler::read() -- file type unsupported");
			return -1;
		default:
			s = IMG_Load_RW(rwops,0);
			break;
	}

	if (element != nullptr)
	{
		log("sdliv::FileHandler::read() -- deleting old element");
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





