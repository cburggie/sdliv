#ifndef _SDLIV_H
#define _SDLIV_H


//linux
#ifndef WIN32
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//windows
#else
#include <sdl2.nuget.2.0.12\build\native\include\SDL.h>
#include <sdl2_image.nuget.2.0.5\build\native\include\SDL_image.h>
#include <sdl2_ttf.v140.2.0.14\build\native\include\SDL_ttf.h>
#endif

#include <sdliv_log.h>

#include <map>
#include <string>
#include <filesystem>
#include <set>




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
		//extern const int window_update_delay_ms;
	}

	typedef enum
	{
		FILETYPE_UNSUPPORTED,
		FILETYPE_ICO,
		FILETYPE_CUR,
		FILETYPE_BMP,
		FILETYPE_GIF,
		FILETYPE_JPG,
		FILETYPE_LBM,
		FILETYPE_PCX,
		FILETYPE_PNG,
		FILETYPE_PNM,
		FILETYPE_SVG,
		FILETYPE_TGA,
		FILETYPE_TIF,
		FILETYPE_XCF,
		FILETYPE_XPM,
		FILETYPE_XV,
		FILETYPE_WEBP
	} ImageFileType;

	class App;
	class Window;
	class Element;
	class Font;
	class FileHandler;


	class App
	{
		private:
			static int _window_event_filter(void * param, SDL_Event * event);

			bool Running;

			Window * window;

			// pointer to image we are currently viewing
			Element * active_element;

			// font rendering object for drawing filenames
			Font * font;

		public:

			//sets pointers to nullptr
			App();

			//calls OnCleanup() if anything is not nullptr
			~App();

			//starts SDL, IMG, TTF makes window and font
			bool OnInit();

			//opens one file and sets it to the active image
			//returns -1 on fail
			int openFile(const char * filepath);
			int openFile(const std::string & filepath);

			//handles main loop
			int OnExecute();

			//handles events, keyboard, mouse, quit, and userevents (if any)
			void OnEvent(SDL_Event* Event);

			//anything that needs to be updated every frame should go here
			//calls window->updateAll()
			void OnLoop();

			//tells window to draw it's contents to the screen
			void OnRender();


			//destroys objects, cleans up, quits SDL, IMG, and TTF
			void OnCleanup();
	};




	class Window
	{
		private:
			static std::map<Uint32,Window*> registeredWindows;
			static bool RegisterWindow(Window * w);
			static int UnregisterWindow(Window * w);

		public:
			static Window * getWindowByID(Uint32 id);
			static Window * getFirstWindow();

		private:

			//SDL objects
			Uint32 SDL_windowID;
			SDL_Renderer *renderer;
			SDL_Window * window;

			//layers maps layer number to elements in that layer
			//layer 1 is active image
			//layer 2 could be image filename text
			std::map<int, std::map<int,Element*>> layers;

			//elements is a map of all elements associated to the window
			std::map<int, Element*> elements;

		public:
			//initializer should open a new SDL_Window
			Window();

			//copy constructor shouldn't really be used, log it!
			Window(const Window & w);

			//make sure all associated Element objects are closed
			~Window();

			SDL_Renderer * getRenderingContext();
			int getWidth() const;
			int getHeight() const;

			SDL_Window* getWindow() const;

			// **FIXME** make sure we obey usable display area
			int setSize(int w, int h);

			Element * createElement(int layer = 0);
			int addElement(Element * e, int layer = 0);
			int removeElement(Element * e);
			int changeElementLayer(Element * e, int layer);

			int resizeElement(Element * e);
			int centerElement(Element * e);

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

			static void setWindowTitle(std::string);
	};


	class Element
	{
		private:
			static int Element_ID_count;
			static int GetNextElementID();

		private:
			bool hidden;
			bool is_copy;
			int ID;
			int xpos;
			int ypos;
			int zpos;

			int width;
			int height;

			double scale;
			double scale_x;
			double scale_y;

			SDL_Rect src_rect; //rectangle to draw from in the texture
			SDL_Rect dst_rect; //rectangle to draw to in the window
			SDL_Surface * surface;
			SDL_Renderer * renderer;
			SDL_Texture * texture;

		public:
			Element();
			Element(const Element & e);
			virtual ~Element();
			int close(); //destroy surface, texture, not renderer

			int setRenderingContext(SDL_Renderer * r);
			SDL_Renderer * getRenderingContext();

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
			int setLayer(int layer);
			int moveDrawPosition(int dy, int dx);

			int setDrawSize(int w, int h);
			int setDrawScale(double s);

			int show();
			int hide();
			virtual int update();
			int draw();
	};





	class Font
	{
		private:
			static bool module_initialized;
			static int ID_count;
			static std::map<int,Font*> font_objects;
			static bool isInit();
		public:
			static int init();
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



/* FileHandler handles all the file io and tracking
 *   It should track files in the directory and load them asynchronously
 *   (eventually), untrack files that get deleted (and unload associated
 *   assets), and reload files that have updated since they were loaded.
 * */

	class FileHandler
	{
		private:
			//master list of all FileHandler instances sorted by filename
			static bool setComparison(const FileHandler *, const FileHandler *);
			static std::set<FileHandler *, decltype(setComparison)*> tracked_files;

			//the active image file that we're viewing in our app
			static FileHandler * active_image;

			//image extensions we support
			static std::set<std::string> supportedExtensions;
			static bool hasValidExtension(const std::filesystem::directory_entry &);

			//start tracking fh in tracked_files
			static int track(FileHandler * fh);

			//stop tracking fh in tracked files
			static std::set<FileHandler*>::iterator untrack(std::set<sdliv::FileHandler*>::iterator fhIter);

			//folder we're looking at for images
			static std::filesystem::directory_entry workingDirectory;

		public:
			//return nullptr if unsupported file type
			static FileHandler* openFileIfSupported(const char * filepath);
			static FileHandler* openFileIfSupported(const std::string & filepath);
			static FileHandler* openFileIfSupported(const std::filesystem::directory_entry & file);

			//begin tracking all files in a directory
			static int openDirectory(bool force = true);

			//get the Element object for the current active image file
			static Element * getActiveImage();

			//advance to the next tracked image file
			static Element * nextImage();

			//backup to the previous tracked image file
			static Element * prevImage();

			static int untrackAll();

			std::string getPathAsString() const;

			static void addSupport(const std::string &extension);

			static std::filesystem::directory_entry getWorkingDirectory();
			static int setWorkingDirectory(std::filesystem::path);
			static int setWorkingDirectory(std::string);


		private:
			ImageFileType type;

			SDL_RWops * rwops;
			Window * window;
			Element * element;

			std::filesystem::directory_entry fs_entry;

			//create rwops or return error
			int open();
			//create element from existing rwops or return error
			int read();
			//destroy rwops or return error if already null
			int close();

		public:
			//null and zero values
			FileHandler();

			//calls setTarget(filename)
			FileHandler(const char * filename);
			FileHandler(const std::string & filename);
			FileHandler(const std::filesystem::directory_entry & file);

			//we should log this because these wont like being copied bitwise
			FileHandler(const FileHandler & fh);

			//make sure rwops is closed, destroy element object
			~FileHandler();

			std::filesystem::path parent_path() const;
			//read image data if file has been updated or if it hasn't been read
			int update();

			//fills fs_entry
			//infers type from filename
			int setTarget(const char *filename);
			int setTarget(const std::string & filename);
			int setTarget(const std::filesystem::directory_entry & file);

			//opens file briefly and uses IMG_isX() to discover image type
			ImageFileType detectImageType();


	};

} //end namespace sdliv

#endif
