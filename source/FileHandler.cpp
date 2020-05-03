#include <sdliv.h>

std::set<std::string> sdliv::FileHandler::supportedExtensions = {
/* added dynamically dependent upon success of Img_Init
	".jpg",
	".png",
	".tif",
*/
	".ico",
	".cur",
	".bmp",
	".gif",
	".lbm",
	".pcx",
	".pnm",
	".svg",
	".xcf",
	".xpm",
	".xv",
	".webp"
};

bool sdliv::FileHandler::hasValidExtension(const std::filesystem::directory_entry &file)
{
	return supportedExtensions.find(file.path().extension().string()) != supportedExtensions.end();
}




//static members
bool sdliv::FileHandler::setComparison(const sdliv::FileHandler *lhs, const sdliv::FileHandler *rhs)
{
	return lhs->getPathAsString() < rhs->getPathAsString();
}
std::set<sdliv::FileHandler*, decltype(sdliv::FileHandler::setComparison)*> sdliv::FileHandler::tracked_files(sdliv::FileHandler::setComparison);

sdliv::FileHandler * sdliv::FileHandler::active_image = nullptr;




//static methods
sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const std::filesystem::directory_entry & dirEnt)
{
	if (active_image != nullptr && active_image->fs_entry == dirEnt)
	{
		return active_image;
	}

	if (!hasValidExtension(dirEnt))
	{
		log("sdliv::FileHandler::openFileIfSupported() -- file extension not supported:", dirEnt.path().filename().string());
		return nullptr;
	}

	FileHandler * fh = new FileHandler(dirEnt);

	if (fh == nullptr)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- failed to allocate FileHandler");
		return nullptr;
	}

	if (fh->type == FILETYPE_UNSUPPORTED)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- unsupported file", dirEnt.path().filename().string());
		delete fh;
		return nullptr;
	}

	if (tracked_files.count(fh) > 0)
	{
		log("sdliv::FileHandler::openFileIfSupported() -- file already tracked", fh->getPathAsString());
		untrack(fh);
	}

	track(fh);

	return fh;
}





sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const std::string & filepath)
{
	return openFileIfSupported(std::filesystem::directory_entry(filepath));
}



sdliv::FileHandler* sdliv::FileHandler::openFileIfSupported(const char * filepath)
{
	return openFileIfSupported(std::string(filepath));
}





int sdliv::FileHandler::track(sdliv::FileHandler * fh)
{
	SDL_assert(fh != nullptr);

	if (tracked_files.find(fh) != tracked_files.end())
	{
		log("sdliv::FileHandler::track() -- file already tracked", fh->getPathAsString());
		return -1;
	}

	tracked_files.insert(fh);
	return 0;
}





int sdliv::FileHandler::untrack(sdliv::FileHandler * fh)
{

	if (tracked_files.erase(fh) == 0)
	{
		log("sdliv::FileHandler::untrack() -- file not tracked");
		return -1;
	}

	if (fh != nullptr) delete fh;
	fh = nullptr;

	return 0;
}





int sdliv::FileHandler::untrackAll()
{
	for (auto & fh : tracked_files)
	{
		delete fh;
	}

	tracked_files.clear();

	return 0;
}



std::string sdliv::FileHandler::getPathAsString() const
{
	return this->fs_entry.path().string();
}



void sdliv::FileHandler::addSupport(const std::string &extension)
{
	supportedExtensions.insert(extension);
}





int sdliv::FileHandler::openDirectory()
{
	int count = 0;

	for (auto& f: std::filesystem::directory_iterator(std::filesystem::current_path()))
	{
		if (f.is_regular_file())
		{
			FileHandler * fh = openFileIfSupported(f);

			if (fh != nullptr)
			{
				count++;
			}
		}
	}

	return count;
}





sdliv::Element * sdliv::FileHandler::getActiveImage()
{
	if (active_image == nullptr)
	{
		if (!tracked_files.empty())
		{
			active_image = *tracked_files.begin();
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
		active_image = *tracked_files.begin();
		return getActiveImage();
	}

	auto iter = tracked_files.find(active_image);
	++iter;

	if (iter == tracked_files.end())
	{
		iter = tracked_files.begin();
	}

	active_image = *iter;

	return getActiveImage();
}





sdliv::Element * sdliv::FileHandler::prevImage()
{
	if (tracked_files.size() == 0)
	{
		log("sdliv::FileHandler::prevImage() -- not tracking any files");
		return nullptr;
	}

	if (active_image == nullptr)
	{
		active_image = *(--(tracked_files.end()));
		return getActiveImage();
	}

	auto i = tracked_files.find(active_image);

	if (i == tracked_files.begin())
	{
		i = tracked_files.end();
	}

	active_image = *(--i);

	return getActiveImage();
}





//non-static methods
sdliv::FileHandler::FileHandler()
{
	type = FILETYPE_UNSUPPORTED;
	rwops = nullptr;
	element = nullptr;
	window = Window::getFirstWindow();
	fs_entry = std::filesystem::directory_entry();
}





sdliv::FileHandler::FileHandler(const std::filesystem::directory_entry & file) : sdliv::FileHandler::FileHandler()
{
	setTarget(file);
}




sdliv::FileHandler::FileHandler(const char * filename) : sdliv::FileHandler::FileHandler(std::string(filename))
{}





sdliv::FileHandler::FileHandler(const std::string & filename) : sdliv::FileHandler::FileHandler(std::filesystem::directory_entry(std::filesystem::current_path() / filename))
{}





sdliv::FileHandler::FileHandler(const sdliv::FileHandler & fh)
{
	// **FIXME** this implementation _will_ cause problems with the destructor
	log("sdliv::FileHandler::FileHandler(const sdliv::FileHandler&) -- copy constructor called");

	type = fh.type;
	rwops = fh.rwops;
	element = fh.element;
	window = fh.window;
	fs_entry = fh.fs_entry;
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
	return setTarget(std::string(fn));
}





int sdliv::FileHandler::setTarget(const std::string & fn)
{
	//filename is _not_ the full path
	std::filesystem::path p = std::filesystem::current_path() / fn;
	return setTarget(std::filesystem::directory_entry(p));
}


int sdliv::FileHandler::setTarget(const std::filesystem::directory_entry & file)
{
	fs_entry = file;
	if (!fs_entry.exists() || !fs_entry.is_regular_file())
	{
		log("sdliv::FileHandler:;setTarget() -- file does not exist", file.path().string());
	}
	type = detectImageType();
	return 0;
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

	if (IMG_isBMP(rwops))  { type = FILETYPE_BMP; }
	else if (IMG_isJPG(rwops))  { type = FILETYPE_JPG; }
	else if (IMG_isPNG(rwops))  { type = FILETYPE_PNG; }
	else if (IMG_isGIF(rwops))  { type = FILETYPE_GIF; }
	else if (IMG_isWEBP(rwops)) { type = FILETYPE_WEBP; }
	else if (IMG_isTIF(rwops))  { type = FILETYPE_TIF; }
	else if (IMG_isSVG(rwops))  { type = FILETYPE_SVG; }
	else if (IMG_isICO(rwops))  { type = FILETYPE_ICO; }
	else if (IMG_isCUR(rwops))  { type = FILETYPE_CUR; }
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
	std::filesystem::file_time_type timestamp = fs_entry.last_write_time();
	if (!std::filesystem::exists(fs_entry))
	{
		log("sdliv::FileHandler::update() -- file no longer exists");
		untrack(this);
		//**FIXME** this will crash, need to update active_image
		return 0;
	}
	fs_entry.refresh();
	if (element == nullptr || fs_entry.last_write_time() > timestamp)
	{
		if (fs_entry.last_write_time() > timestamp)
		{
			log("sdliv::FileHandler::update() -- file changed since last read");
		}

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

	rwops = nullptr;

	return error;
}





