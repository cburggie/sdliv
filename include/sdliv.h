#ifndef _SDLIV_H
#define _SDLIV_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <map>
#include <string>




/*	App objects set up and tear down the program and handle events and such
 *		makes a Window object
 *		can open image files
 *		can open a directory of images
 *		each image is wrapped by an Element
 *
 *	Window objects are a open window on your desktop
 *		tracks all the associated Element objects
 *		Element objects are meaningless without the associated Window,
 *			so Elements should be created and destroyed by the associated Window
 *		Drawing of elements is handled in layers
 *
 *	Element objects wrap a texture that can be drawn into a window
 *		can be created from a file path or an SDL_Surface or text
 *		Elements should be created and destroyed by the associated window
 *
 *	Font objects render input text to an SDL_Surface
 *		each font object renders a specific font at a specific font size
 *		Font::Init() initializes the font rendering subsystem
 *		create a Font object with Font::openFont(window,path,size)
 */





namespace sdliv
{
	namespace constants
	{
		extern const char * window_title;
		extern const char * font_path;
		extern const int window_minimum_width;
		extern const int window_minimum_height;
	}

	class App;
	class Window;
	class Element;
	class Font;

	void log(const char * text);
	void log(const std::string & text);



	class App
	{
		private:
			bool Running;

			Window * window;

			Element * active_element;
			std::map<int, Element*> elements;

			Font * font;

		public:

			//sets pointers to NULL
			App();

			//calls OnCleanup() if anything is not NULL
			~App();

			//starts SDL, IMG, TTF makes window and font
			bool OnInit();

			//opens one file and sets it to the active index
			int openFile(const char * filepath);
			int openFile(const std::string & filepath);

			//loads all images in path into Element objects stored in `elements`
			//should do it's work in a separate thread?
			int openDirectory(const char * path);
			int openDirectory(const std::string & path);

			//handles main loop
			int OnExecute();

			//handles events, keyboard, mouse, quit, and userevents (if any)
			void OnEvent(SDL_Event* Event);

			//anything that needs to be updated every frame should go here
			//calls window->updateAll()
			void OnLoop();

			//tells window to draw it's contents to the screen
			void OnRender();


			//destroys cleans up, quits SDL, IMG, and TTF
			void OnCleanup();
	};




	class Window
	{
		private:
			int ID_count;

			SDL_Renderer *renderer;
			SDL_Window * window;

			int width;
			int height;

			std::map<int, std::map<int,Element*>> layers;
			std::map<int, Element*> elements;

		public:
			Window();
			Window(const Window & w); //this really shouldn't ever get called
			~Window();

			SDL_Renderer * getRenderingContext();
			int getWidth() const;
			int getHeight() const;
			int setSize(int w, int h);

			Element * createElement(int layer = 0);
			int addElement(Element * e, int layer = 0);
			int changeElementLayer(Element * e, int layer);

			const int getNewElementID();

			int updateAll();
			int updateLayer(int layer);
			int updateElement(Element * e);
			int updateElement(const int ID);

			int setBackgroundColor(int r, int g, int b, int a = 255);

			int clear(); //clears to background color
			int drawAll(); //draws all elements, layer by layer
			int drawLayer(int layer); //draws one layer of elements
			int drawElement(Element * e); //draws one element
			int drawElement(const int ID); // ""
			int present(); //present screen updates on display
	};


	class Element
	{
		private:
			const int ID;
			int xpos;
			int ypos;
			int zpos;

			int width;
			int height;

			double scale;

			SDL_Rect src_rect; //rectangle to draw from in the texture
			SDL_Rect dst_rect; //rectangle to draw to in the window
			SDL_Surface * surface;
			SDL_Renderer * renderer;
			SDL_Texture * texture;

		public:
			Element();
			Element(const Element & e);
			~Element();
			int close();

			int setRenderingContext(SDL_Renderer * r);

			int createFromSurface(SDL_Surface * s);
			int createFromImage(const char * path);
			int createFromImage(const std::string & path);
			int createFromText(Font * font, const char * txt);
			int createFromText(Font * font, const std::string & txt);

			int getID() const;
			int getWidth() const;
			int getHeight() const;
			int getLayer() const;

			double getDrawScale() const;
			int getDrawWidth() const;
			int getDrawHeight() const;
			int getDrawXPos() const;
			int getDrawYPos() const;

			int setDrawPosition(int y, int x);
			int moveDrawPosition(int dy, int dx);

			int setDrawSize(int w, int h);
			int setDrawScale(double s);

			int update();
			int draw();
	};





	class Font
	{
		private:
			static bool module_initialized;
			static int ID_count;
			static std::map<int,Font*> font_objects;
			static int init();
			static bool isInit();
			static int quit();
			static Font * openFont(Window * window, const char * path, int font_size = 12);
			static Font * openFont(Window * window, const std::string & path, int font_size = 12);

		private:
			int ID;
			int font_height;
			int font_size;
			TTF_Font * font;
			SDL_Renderer * renderer;
			SDL_Color c;

		public:
			Font();
			Font(const Font & f);
			~Font();
			int close();

			int setColor(int r, int g, int b, int a = 255);
			int getHeight() const;
			int getFontSize() const;

			void getTextSize(const char * text, int * w, int * h);
			void getTextSize(const std::string & txt, int * w, int * h);

			SDL_Surface * renderText(const char * txt);
			SDL_Surface * renderText(const std::string & txt);
	};
}

#endif
