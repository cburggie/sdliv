#include <sdliv.h>





sdliv::App::App()
{
	Running = false;
	active_element = nullptr;
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
	FileHandler * fh = FileHandler::openFileIfSupported(filepath);

	if (fh == nullptr)
	{
		log("sdliv::App::openFile() -- file type not supported",filepath);
		return -1;
	}

	std::filesystem::current_path(fh->parent_path());
	active_element = FileHandler::getActiveImage();

	SDL_assert(active_element != nullptr);

	window->setSize(active_element->getWidth(), active_element->getHeight());
	SDL_ShowWindow(window->getWindow());
	window->centerElement(active_element);

	return 0;
}





int sdliv::App::openFile(const std::string & s)
{
	return openFile(s.c_str());
}





int sdliv::App::OnExecute()
{
	Running = true;
	SDL_Event e;

	OnRender();

	//Custom Timer Events go here

	SDL_AddEventWatch([](void * appV, SDL_Event *event)-> int
	{
		if (event->type == SDL_WINDOWEVENT)
		{
			sdliv::App *app = (sdliv::App*)appV;
			app->window->centerElement(app->active_element);
			app->OnRender();
		}
		return 0; //ignored
	},
		this
		);

	while (Running)
	{
		if (SDL_WaitEvent(&e) != 1)
		{
			log("sdliv::App::OnExecute() -- SDL_WaitEvent returned error",SDL_GetError());
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
	FileHandler::untrackAll();

	active_element = nullptr;

	SDL_assert(window != nullptr);
	delete window;
	window = nullptr;

	SDL_assert(font != nullptr);
	font->close();
	delete font;
	font = nullptr;
	Font::quit();

	IMG_Quit();
	SDL_Quit();
}
