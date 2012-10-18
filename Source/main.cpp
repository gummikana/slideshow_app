#include <iostream>
#include <sstream>
#include "include_poro.h"
#include "slideshow.h"


//-----------------------------------------------------------------------------


int main( int argc, char** args )
{
	poro::AppConfig app_conf;
	app_conf.title = "Prototyping for Game Design";
	/*app_conf.fullscreen = 1;
	app_conf.window_w = 2048;
	app_conf.window_h = 768 * 2;*/

	poro::RunPoro< SlideShowApp >( app_conf );

	return 0;
}
