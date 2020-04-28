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

	Uint32 img_init_flags = 0;
	img_init_flags |= IMG_INIT_PNG;
	img_init_flags |= IMG_INIT_JPG;
	img_init_flags |= IMG_INIT_TIF;
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
	active_element->createFromImage(filepath);

	elements[active_element->getID()] = activeElement;
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





int sdliv::App::OnExecute()
{
	Running = true;
	SDL_Event e;

	OnRender();

	//Custom Timer Events go here

	while (Running)
	{
		if (SDL_WaitEvent(&e))
		{
			log("sdliv::App::OnExecute() -- SDL_WaidEvent returned error");
			log(SDL_GetError());
			Running = false;
		}
		else
		{
			OnEvent(&e)
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

	window->clear();
	window->drawElement(active_element);
	window->present();
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
