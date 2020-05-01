#include <sdliv.h>



void sdliv::App::OnEvent(SDL_Event * e)
{
	SDL_assert(e != nullptr);

	switch (e->type)
	{
		/*
		case SDL_WINDOWEVENT:
			switch (e->window.event)
			{
				case SDL_WINDOWEVENT_SHOWN:
					log("SDL_WINDOWEVENT_SHOWN");
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					log("SDL_WINDOWEVENT_HIDDEN");
					break;
				case SDL_WINDOWEVENT_EXPOSED:
					log("SDL_WINDOWEVENT_EXPOSED");
					break;
				case SDL_WINDOWEVENT_MOVED:
					log("SDL_WINDOWEVENT_MOVED");
					break;
				case SDL_WINDOWEVENT_RESIZED:
					log("SDL_WINDOWEVENT_RESIZED");
					break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					log("SDL_WINDOWEVENT_SIZE_CHANGED");
					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					log("SDL_WINDOWEVENT_MINIMIZED");
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					log("SDL_WINDOWEVENT_MAXIMIZED");
					break;
				case SDL_WINDOWEVENT_RESTORED:
					log("SDL_WINDOWEVENT_RESTORED");
					break;
				case SDL_WINDOWEVENT_ENTER:
					log("SDL_WINDOWEVENT_ENTER");
					break;
				case SDL_WINDOWEVENT_LEAVE:
					log("SDL_WINDOWEVENT_LEAVE");
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					log("SDL_WINDOWEVENT_FOCUS_GAINED");
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					log("SDL_WINDOWEVENT_FOCUS_LOST");
					break;
				case SDL_WINDOWEVENT_CLOSE:
					log("SDL_WINDOWEVENT_CLOSE");
					break;
				case SDL_WINDOWEVENT_TAKE_FOCUS:
					log("SDL_WINDOWEVENT_TAKE_FOCUS");
					break;
				case SDL_WINDOWEVENT_HIT_TEST:
					log("SDL_WINDOWEVENT_HIT_TEST");
					break;
				default:
					break;
			}
			break;
			*/
		case SDL_KEYDOWN:
			switch (e->key.keysym.sym)
			{
				case SDLK_LEFT:
					active_element = FileHandler::prevImage();
					window->resizeElement(active_element);
					window->centerElement(active_element);
					OnRender();
					break;
				case SDLK_RIGHT:
					active_element = FileHandler::nextImage();
					window->resizeElement(active_element);
					window->centerElement(active_element);
					OnRender();
					break;
				case SDLK_q:
					Running = false;
					break;
				default:
					break;
			}
			break;
		case SDL_QUIT:
			Running = false;
			break;
		default:
			break;
	}
}
