#include <sdliv.h>

std::map<Uint32,sdliv::Window*> sdliv::Window::registeredWindows = std::map<Uint32,sdliv::Window*>();

bool sdliv::Window::RegisterWindow(Window * w)
{
	SDL_assert(registeredWindows.count(w->SDL_windowID) == 0);
	registeredWindows[w->SDL_windowID] = w;
	return true;
}
int sdliv::Window::UnregisterWindow(Window * w)
{
	SDL_assert(w != nullptr);
	Uint32 c = registeredWindows.erase(w->SDL_windowID);
	SDL_assert(c > 0);
	return 0;
}

sdliv::Window* sdliv::Window::getWindowByID(Uint32 id)
{
	return registeredWindows[id];
}

//initializer should open a new SDL_Window
sdliv::Window::Window()
{
	window = nullptr;
	renderer = nullptr;
	///**FIXME** verify this works with x=0, y=0
	SDL_CreateWindowAndRenderer(0, 0, 0, &(window), &(renderer));
	RegisterWindow(this);
}

//copy constructor shouldn't really be used, log it!
sdliv::Window::Window(const sdliv::Window & w)
{
	sdliv::log("Error: call to Window(const Window & w)");
}

//make sure all associated Element objects are closed
sdliv::Window::~Window()
{
	for (auto iter = elements.begin();
		iter != elements.end();
		++iter)
	{
		iter->second->close();
		delete iter->second;
	}
}

SDL_Renderer* sdliv::Window::getRenderingContext()
{
	SDL_assert(renderer != nullptr);
	return renderer;
}
int sdliv::Window::getWidth() const
{
	SDL_assert(window != nullptr);
	int w;
	SDL_GetWindowSize(window, &w, nullptr);
	return w;
}
int sdliv::Window::getHeight() const
{
	SDL_assert(window != nullptr);
	int h;
	SDL_GetWindowSize(window, nullptr, &h);
	return h;
}
int sdliv::Window::setSize(int w, int h)
{
	SDL_assert(window != nullptr);
	SDL_SetWindowSize(window, w, h);
	return 0;
}

sdliv::Element* sdliv::Window::createElement(int layer)
{
	Element *element = new Element();
	SDL_assert(renderer != nullptr);
	element->setRenderingContext(renderer);
	element->setLayer(layer);
	elements[element->getID()] = element;
	layers[layer][element->getID()] = element;
	return element;
}

int sdliv::Window::addElement(sdliv::Element * e, int layer)
{
	SDL_assert(e->getRenderingContext() != nullptr);
	e->setRenderingContext(renderer);
	elements[e->getID()] = e;
	layers[layer][e->getID()] = e;
	return 0;
}

int sdliv::Window::changeElementLayer(sdliv::Element * e, int layer)
{
	SDL_assert(e != nullptr);
	layers[e->getLayer()].erase(e->getID());
	layers[layer][e->getID()] = e;
	e->setLayer(layer);
	return 0;
}

int sdliv::Window::updateAll()
{
	for (const auto& e : elements)
//	for (const auto& [key, value] : elements)
//	for (auto iter = elements.begin();
//		iter != elements.end();
//		++iter)
	{
		e.second->update();
	}
	return 0;
}

int sdliv::Window::updateLayer(int layer)
{
	for (auto iter = layers[layer].begin();
		iter != layers[layer].end();
		++iter)
	{
		iter->second->update();
	}
	return 0;
}

int sdliv::Window::updateElement(sdliv::Element * e)
{
	SDL_assert(e != nullptr);
	e->update();
	return 0;
}
int sdliv::Window::updateElement(const int ID)
{
	SDL_assert(elements.count(ID) > 0);
	elements[ID]->update();
	return 0;
}

int sdliv::Window::setBackgroundColor(int r, int g, int b, int a)
{
	SDL_assert(renderer != nullptr);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	return 0;
}

//clears to background color
int sdliv::Window::clear()
{
	SDL_assert(renderer != nullptr);
	return SDL_RenderClear(renderer);
}
//draws all elements, layer by layer
int sdliv::Window::drawAll()
{
	for (auto layer_iter = layers.begin();
		layer_iter != layers.end();
		++layer_iter)
	{
		drawLayer(layer_iter->first);
	}
	return 0;
}
//draws one layer of elements
int sdliv::Window::drawLayer(int layer)
{
	for (auto element_iter = layers[layer].begin();
		element_iter != layers[layer].end();
		++element_iter)
	{
		element_iter->second->draw();
	}
	return 0;
}
//draws one element
int sdliv::Window::drawElement(sdliv::Element * e) //draws one element
{
	SDL_assert(e != nullptr);
	e->draw();
	return 0;
}

// ""
int sdliv::Window::drawElement(const int ID)
{
	SDL_assert(elements.count(ID) > 0);
	elements[ID]->draw();
	return 0;
}

//present screen updates on display
int sdliv::Window::present()
{
	SDL_RenderPresent(renderer);
	return 0;
}
