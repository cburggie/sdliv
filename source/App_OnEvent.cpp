#include <sdliv.h>



void sdliv::App::OnEvent(SDL_Event * e)
{
	SDL_assert(e != nullptr);

	switch (e->type)
	{
		case SDL_WINDOWEVENT:
			window->centerElement(active_element);
			OnRender();
			break;
		case SDL_KEYDOWN:
			switch (e->key.keysym.sym)
			{
				case SDLK_LEFT:
					active_element = FileHandler::prevImage();
					window->centerElement(active_element);
					OnRender();
					break;
				case SDLK_RIGHT:
					active_element = FileHandler::nextImage();
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
