#ifndef WIN32
#include <SDL2/SDL.h>
#else
#include <sdl2.2.0.5\build\native\include\SDL.h>
#endif

#include <sdliv.h>





int sdliv::Element::Element_ID_count = 0;





int sdliv::Element::GetNextElementID()
{
	return ++Element_ID_count;
}





sdliv::Element::Element()
{
	hidden = true;
	is_copy = false;
	ID = GetNextElementID();
	xpos = 0;
	ypos = 0;
	zpos = 0;

	width = 0;
	height = 0;

	scale = 1.0;

	src_rect = { 0,0,0,0 };
	dst_rect = { 0,0,0,0 };

	surface = nullptr;
	renderer = nullptr;
	texture = nullptr;
}





sdliv::Element::Element(const Element & e)
{
	log("calling sdliv::Element::Element(const Element&)...");
	hidden = e.hidden;
	is_copy = true;
	
	ID = e.ID;
	xpos = e.xpos;
	ypos = e.ypos;
	zpos = e.zpos;

	width = e.width;
	height = e.height;

	scale = e.scale;

	src_rect.x = e.src_rect.x;
	src_rect.y = e.src_rect.y;
	src_rect.w = e.src_rect.w;
	src_rect.h = e.src_rect.h;

	dst_rect.x = e.dst_rect.x;
	dst_rect.y = e.dst_rect.y;
	dst_rect.w = e.dst_rect.w;
	dst_rect.h = e.dst_rect.h;

	surface = e.surface;
	renderer = e.renderer;
	texture = e.texture;
}





sdliv::Element::~Element()
{
	if (texture != nullptr || surface != nullptr)
	{
		close();
	}
}





int sdliv::Element::close()
{
	if (is_copy)
	{
		log("sdliv::Element::close() element is a copy of another");
		return -1;
	}

	int error = -1;

	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
		hidden = true;
		error = 0;
	}

	if (surface != nullptr)
	{
		SDL_FreeSurface(surface);
		surface = nullptr;
		hidden = true;
		error = 0;
	}

	return error;
}





int sdliv::Element::setRenderingContext(SDL_Renderer * r)
{
	renderer = r;
	return r != nullptr;
}





SDL_Renderer * sdliv::Element::getRenderingContext()
{
	return renderer;
}





int sdliv::Element::createFromSurface(SDL_Surface * s)
{
	if (s == nullptr)
	{
		log("sdliv::Element::createFromSurface() -- passed null parameter");
		return -1;
	}

	surface = s;
	if (renderer != nullptr)
	{
		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTextureFromSurface(renderer,surface);
		if (texture == nullptr)
		{
			log("sdliv::Element::createFromSurface() -- SDL_CreateTextureFromSurface() failed");
			close();
			return -1;
		}

		hidden = false;
		width = s->w; height = s->h;
		xpos = ypos = zpos = 0;
		src_rect.x = 0; src_rect.y = 0; src_rect.w = width; src_rect.h = height;
		dst_rect.x = 0; dst_rect.y = 0; dst_rect.w = width; dst_rect.h = height;
	}

	else
	{
		log("sdliv::Element::createFromSurface() called with no rendering context");
		return -1;
	}

	return 0;
}





int sdliv::Element::createFromImage(const char * path)
{
	if (is_copy)
	{
		log("sdliv::Element::createFromImage() called from copy");
		return -1;
	}

	if (texture != nullptr || surface != nullptr)
	{
		log("sdliv::Element::createFromImage() called with unclosed texture");
		close();
	}

	surface = IMG_Load(path);
	if (surface == nullptr)
	{
		log("sdliv::Element::createFromImage() -- IMG_Load failed");
		log(path);
		return -1;
	}

	return createFromSurface(surface);
}





int sdliv::Element::createFromImage(const std::string & path)
{
	return createFromImage(path.c_str());
}





int sdliv::Element::createFromText(Font * font, const char * txt)
{
	if (is_copy)
	{
		log("sdliv::Element::createFromText() called from copy");
		return -1;
	}

	if (texture != nullptr || surface != nullptr)
	{
		log("sdliv::Element::createFromText() called with unclosed texture");
		close();
	}
	
	if (font == nullptr)
	{
		log("sdliv::Element::createFromText() called with null font parameter");
		return -1;
	}

	return createFromSurface(font->renderText(txt));
}





int sdliv::Element::createFromText(Font * font, const std::string & txt)
{
	return createFromText(font, txt.c_str());
}





int sdliv::Element::getID() const
{
	return ID;
}





int sdliv::Element::getWidth() const
{
	return width;
}





int sdliv::Element::getHeight() const
{
	return height;
}





int sdliv::Element::getLayer() const
{
	return zpos;
}





double sdliv::Element::getDrawScale() const
{
	return scale;
}





int sdliv::Element::getDrawWidth() const
{
	return dst_rect.w;
}





int sdliv::Element::getDrawHeight() const
{
	return dst_rect.h;
}





int sdliv::Element::getDrawXPos() const
{
	return dst_rect.x;
}





int sdliv::Element::getDrawYPos() const
{
	return dst_rect.y;
}





int sdliv::Element::setDrawPosition(int y, int x)
{
	dst_rect.x = x; dst_rect.y = y;
	return 0;
}





int sdliv::Element::setLayer(int layer)
{
	zpos = layer;
	return 0;
}





int sdliv::Element::moveDrawPosition(int dy, int dx)
{
	dst_rect.x += dx; dst_rect.y += dy;
	return 0;
}





int sdliv::Element::setDrawSize(int w, int h)
{
	if (w <= 0 || h <= 0)
	{
		log("sdliv::Element::setDrawSize() called with negative params");
		return -1;
	}

	dst_rect.w = w; dst_rect.h = h;

	scale_x = ((double) w) / width;
	scale_y = ((double) h) / height;

	scale = scale_x < scale_y ? scale_x : scale_y;

	return 0;
}





int sdliv::Element::setDrawScale(double s)
{
	if (s < 0.00001)
	{
		log("sdliv::Element::setDrawScale() called with very small param");
		return -1;
	}

	scale = scale_x = scale_y = s;
	dst_rect.w = (int) (scale_x * width + 0.5); //rounds to nearest pixel
	dst_rect.h = (int) (scale_y * height + 0.5); //rounds to nearest pixel

	return 0;
}





int sdliv::Element::show()
{
	if (!hidden) return -1;

	hidden = false;
	return 0;
}





int sdliv::Element::hide()
{
	if (hidden) return -1;

	hidden = true;
	return 0;
}





int sdliv::Element::update()
{
	return 0;
}





int sdliv::Element::draw()
{
	if (renderer == nullptr || texture == nullptr)
	{
		log("sdliv::Element::draw() called with null renderer or texture member");
		return -1;
	}

	return SDL_RenderCopy(renderer,texture,&src_rect,&dst_rect);
}
