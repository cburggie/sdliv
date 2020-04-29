#include <sdliv.h>



std::map<Uint32,sdliv::Window*> sdliv::Window::registeredWindows
		= std::map<Uint32,sdliv::Window*>();



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

	SDL_CreateWindowAndRenderer(
			0, 0, 
			SDL_WINDOW_HIDDEN, 
			&(window), &(renderer));

	RegisterWindow(this);
	setBackgroundColor(0,0,0);
}



//copy constructor shouldn't really be used, log it!
sdliv::Window::Window(const sdliv::Window & w)
{
	sdliv::log("Error: call to Window(const Window & w)");
}



//make sure all associated Element objects are closed
//iterate over key-value pairs in elements to delete all Element objects
sdliv::Window::~Window()
{
	for(auto & p : elements)
	{
		p.second->close();
		delete p.second;
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

SDL_Window * sdliv::Window::getWindow() const
{
	return window;
}



int sdliv::Window::setSize(int w, int h)
{
	SDL_assert(window != nullptr);

	SDL_SetWindowSize(window, w, h);

	return 0;
}



sdliv::Element* sdliv::Window::createElement(int layer)
{
	SDL_assert(renderer != nullptr);

	Element *element = new Element();

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





int sdliv::Window::centerElement(Element * e)
{
	SDL_assert(e != nullptr);

	int y_offset = (getHeight() / 2) - (e->getDrawHeight() / 2);
	int x_offset = (getWidth() / 2) - (e->getDrawWidth() / 2);

	e->setDrawPosition(y_offset, x_offset);

	return 0;
}



//iterate over key-value pairs in elements to update all Element objects
int sdliv::Window::updateAll()
{
	for (auto & p : elements)
	{
		p.second->update();
	}

	return 0;
}



//iterate over key-value pairs in layer to update elements
int sdliv::Window::updateLayer(int layer)
{
	for (auto & p : layers[layer])
	{
		p.second->update();
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

	if (SDL_SetRenderDrawColor(renderer, r, g, b, a))
	{
		log("sdliv::Window::setBackgroundColor failed");
		log(SDL_GetError());
	}

	return 0;
}



//clears to background color
int sdliv::Window::clear()
{
	SDL_assert(renderer != nullptr);

	return SDL_RenderClear(renderer);
}



//draws all elements, layer by layer
//iterate over key-value pairs in layers to draw layers
int sdliv::Window::drawAll()
{
	for (auto & p : layers)
	{
		drawLayer(p.first);
	}

	return 0;
}



//draws one layer of elements
//iterate over key-value pairs in layer to draw Elements
int sdliv::Window::drawLayer(int layer)
{
	for (auto & p : layers[layer])
	{
		drawElement(p.second);
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
	SDL_assert(elements[ID] != nullptr);

	elements[ID]->draw();

	return 0;
}



//present screen updates on display
int sdliv::Window::present()
{
	SDL_RenderPresent(renderer);

	return 0;
}
