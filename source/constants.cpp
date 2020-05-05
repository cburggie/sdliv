#include <sdliv.h>

const char * sdliv::constants::window_title = "Charlotte's Amazing SDL Image Viewer - ";
#ifndef WIN32
const char * sdliv::constants::font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
#else
const char * sdliv::constants::font_path = "c:\\windows\\fonts\\DejaVuSansMono.ttf";
#endif
const int sdliv::constants::window_minimum_width = 100;
const int sdliv::constants::window_minimum_height = 50;
//const int sdliv::constants::window_update_delay_ms = 50;

