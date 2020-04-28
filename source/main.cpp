#include <sdliv.h>

#ifndef WIN32
const char * image_path = "./image.png";
#else
const char * image_path = "E:\\Programming Projects\\ImageViewer\\image.jpg";
#endif

int main(int argc, char * argv[])
{
	sdliv::App app;
	app.OnInit();
	app.openFile(image_path);
	return app.OnExecute();
}
