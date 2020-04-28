#include <sdliv.h>

#ifndef WIN32
const char * image_path = "./image.png";
#else
const char * image_path = "";
#endif

int main(int argc, char * argv[])
{
	sdliv::App app;
	sdliv::log(SDL_GetError());
	app.OnInit();
	sdliv::log(SDL_GetError());
	app.openFile(image_path);
	sdliv::log(SDL_GetError());
	return app.OnExecute();
}
