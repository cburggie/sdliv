#include <sdliv.h>





sdliv::App::App()
{
	Running = false;
	active_element = nullptr;
	elements = std::map<int,Element*>();
	window = nullptr;
	font = nullptr;
}





sdliv::App::~App()
{
	if (font != nullptr || window != nullptr)
	{
		log("sdliv::App::~App() called before OnCleanup()");
		OnCleanup();
	}
}





bool sdliv::App::OnInit()
{
	Uint32 sdl_init_flags = 0;
	sdl_init_flags |= SDL_INIT_TIMER;
	sdl_init_flags |= SDL_INIT_VIDEO;
	sdl_init_flags |= SDL_INIT_EVENTS;
	if (SDL_Init(sdl_init_flags))
	{
		log("sdliv::App::OnInit() -- SDL_Init() returned error");
		log(SDL_GetError());
		return true;
	}

	int img_init_flags = 0;
#ifndef WIN32
	img_init_flags |= IMG_INIT_PNG;
#endif
	img_init_flags |= IMG_INIT_JPG;
#ifndef WIN32
	img_init_flags |= IMG_INIT_TIF;
#endif

	if (img_init_flags != IMG_Init(img_init_flags))
	{
		log("sdliv::App::OnInit() -- IMG_Init() failed at least partially");
		log(IMG_GetError());
		SDL_Quit();
		return true;
	}

	window = new Window();
	SDL_assert(window != nullptr);

	Font::init();
	font = Font::openFont(window, constants::font_path);
	SDL_assert(font != nullptr);

	return false;
}





int sdliv::App::openFile(const char * filepath)
{
	SDL_assert(window != nullptr);
	active_element = window->createElement();

	SDL_assert(active_element != nullptr);
	if (active_element->createFromImage(filepath))
	{
		log("createFromImageFile returned an error code");
	}

	elements[active_element->getID()] = active_element;
	window->setSize(active_element->getWidth(), active_element->getHeight());
	window->centerElement(active_element);

	return 0;
}





int sdliv::App::openFile(const std::string & s)
{
	return openFile(s.c_str());
}





int sdliv::App::openDirectory(const char * path)
{
	// **FIXME** implement this
	return 0;
}


int sdliv::App::openDirectory(const std::string & path)
{
	return openDirectory(path.c_str());
}

/*
void sdliv::App::OnEvent(SDL_Event *e)
{
	SDL_assert(e != nullptr);
	switch (e->type)
	{
	case SDL_QUIT:
		Running = false;
		break;
	case SDL_WINDOWEVENT:
//		window->updateAll();
		OnRender();
		break;
	default:
		break;
	}
}
*/

int sdliv::App::OnExecute()
{
	Running = true;
	SDL_Event e;

	OnRender();

	//Custom Timer Events go here

	while (Running)
	{
		if (SDL_WaitEvent(&e) != 1)
		{
			log("sdliv::App::OnExecute() -- SDL_WaitEvent returned error");
			log(SDL_GetError());
		}
		else
		{
			OnEvent(&e);
		}
	}

	OnCleanup();

	return 0;
}





void sdliv::App::OnLoop()
{
	SDL_assert(window != nullptr);

	window->updateAll();
}





void sdliv::App::OnRender()
{
	SDL_assert(window != nullptr);
	SDL_assert(active_element != nullptr);

	if (window->clear())
	{
		log("onrender() failed at window->clear()");
		log(SDL_GetError());
	}
	if (window->drawElement(active_element))
	{
		log("onrender() failed at window->drawElement()");
		log(SDL_GetError());
	}
	if (window->present())
	{
		log("onrender() failed at window->present()");
		log(SDL_GetError());
	}
}





void sdliv::App::OnCleanup()
{
	elements.clear();

	active_element = nullptr;

	SDL_assert(window != nullptr);
	delete window;
	window = nullptr;

	SDL_assert(font != nullptr);
	delete font;
	font = nullptr;
	Font::quit();

	IMG_Quit();
	SDL_Quit();
}
