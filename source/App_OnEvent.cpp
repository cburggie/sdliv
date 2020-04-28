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
		case SDL_QUIT:
			Running = false;
			break;
		default:
			break;
	}
}
