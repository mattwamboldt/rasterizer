#include <SDL/SDL.h>
#include <stdio.h>
#include "debug.h"
#include "perftimer.h"

#include "rendering\tests.h"
#include "rendering\3d\mesh.h"
#include "rendering\device.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
Mesh gMesh;
Device* gDevice;

//Starts up SDL and creates window
bool init();
bool loadMedia();
void close();

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
		Debug::console("Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            Debug::console("Failed to load media!\n" );
        }
        else
        {
			//Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}
					
                gDevice->Clear(Color(0x000000));
                Draw(gDevice, gMesh);
				SDL_UpdateWindowSurface( gWindow );
			}
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        Debug::console("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            Debug::console("Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
            gDevice = new Device(gScreenSurface);
        }
    }

	PerfTimer::Init();

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    if (!gMesh.ReadTestFormat("data/suzanne.obj"))
    {
        Debug::console("Unable to load obj file %s! SDL Error: %s\n", "data/suzanne.obj", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    //Deallocate surface
    if (gDevice)
    {
        delete gDevice;
    }

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}