#include <sdliv.h>
#include <string>


bool sdliv::Font::module_initialized = false;
int sdliv::Font::ID_count = 0;
std::map<int,sdliv::Font*> sdliv::Font::font_objects = std::map<int,sdliv::Font*>();





int sdliv::Font::init()
{
	if (module_initialized || TTF_Init())
	{
		return -1;
	}

	module_initialized = true;
	return 0;
}





bool sdliv::Font::isInit()
{
	return module_initialized;
}





int sdliv::Font::quit()
{
	if (!module_initialized)
	{
		log("sdliv::Font::Quit() called while module uninitialized");
		return -1;
	}

	//destroy all remaining open font objects
	for (auto && p : font_objects)
	{
		Font * f = p.second;
		if (f != nullptr)
		{
			f->close();
			delete f;
		}
	}

	font_objects.clear();
	module_initialized = false;
	TTF_Quit();

	return 0;
}





sdliv::Font * sdliv::Font::openFont(Window * w, const std::string & path, int fs)
{
	return openFont(w,path.c_str(),fs);
}





sdliv::Font * sdliv::Font::openFont(Window * w, const char * path, int fs)
{
	Font * fp = new Font();
	SDL_assert(fp != nullptr);

	fp->font = TTF_OpenFont(path, fs);

	if (fp->font == nullptr)
	{
		log("sdliv::Font::openFont() failed at TTF_OpenFont()");
		log("\tpath was: ", path);

		delete fp;
		return nullptr;
	}

	fp->ID = ++ID_count;
	fp->renderer = w->getRenderingContext();
	fp->font_size = fs;
	fp->font_height = TTF_FontHeight(fp->font) + TTF_FontLineSkip(fp->font);

	font_objects[fp->ID] = fp;

	return fp;
}


sdliv::Font::Font()
{
	ID = 0;
	font_height = 0;
	font_size = 0;
	font = nullptr;
	renderer = nullptr;
	c = {255,255,255,255};
}


sdliv::Font::Font(const Font & f)
{
	log("sdliv::Font copy constructor called");
	ID = 0;
	font_height = 0;
	font_size = 0;
	font =  nullptr;
	renderer = nullptr;
	c = { 255,255,255,255 };
}





sdliv::Font::~Font()
{
	bool error = false;
	if (font != nullptr)
	{
		log("sdliv::Font::~Font() called with non-null `font` member");
		error = true;
	}

	if (renderer != nullptr)
	{
		log("sdliv::Font::~Font() called with non-null `renderer` member");
		error = true;
	}

	if (error) close();
}





int sdliv::Font::close()
{
	bool error = false;
	if (font == nullptr)
	{
		log("sdliv::Font::close() called with null `font` member");
		error = true;
	}

	if (!module_initialized)
	{
		log("sdliv::Font::close() called while module uninitialized");
		error = true;
	}

	if (error) return -1;

	TTF_CloseFont(font);
	font = nullptr;
	renderer = nullptr;
	font_objects.erase(ID);

	return 0;
}





int sdliv::Font::setColor(int r, int g, int b, int a)
{
	bool error = false;

	if (r < 0 || r > 255)
	{
		log("sdliv::Font::setColor() called with out of bounds `r` parameter");
		error = true;
	}

	if (g < 0 || g > 255)
	{
		log("sdliv::Font::setColor() called with out of bounds `g` parameter");
		error = true;
	}

	if (b < 0 || b > 255)
	{
		log("sdliv::Font::setColor() called with out of bounds `b` parameter");
		error = true;
	}

	if (a < 0 || a > 255)
	{
		log("sdliv::Font::setColor() called with out of bounds `a` parameter");
		error = true;
	}

	if (error) return -1;

	c.r = r; c.g = g; c.b = b; c.a = a;
	return 0;
}





int sdliv::Font::getHeight() const { return font_height; }
int sdliv::Font::getFontSize() const { return font_size; }





void sdliv::Font::getTextSize(const std::string & txt, int * w, int * h)
{
	getTextSize(txt.c_str(),w,h);
}





void sdliv::Font::getTextSize(const char * txt, int * w, int * h)
{
	TTF_SizeText(font,txt,w,h);
}





SDL_Surface * sdliv::Font::renderText(const char * txt)
{
	return TTF_RenderText_Blended(font,txt,c);
}





SDL_Surface * sdliv::Font::renderText(const std::string & txt)
{
	return renderText(txt.c_str());
}
