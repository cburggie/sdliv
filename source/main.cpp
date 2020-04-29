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

	if (argc > 1)
	{
		app.openFile(argv[1]);
	}

	else
	{
		app.openFile(image_path);
	}

	return app.OnExecute();
}
