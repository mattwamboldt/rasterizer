#include <SDL/SDL.h>
#include <stdio.h>
#include "debug.h"
#include "audio/audio.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

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

			if (Audio::Init())
			{
				SDL_Rect imgRect;
				SDL_Rect screenRect;
				SDL_GetClipRect(gHelloWorld, &imgRect);
				SDL_GetClipRect(gScreenSurface, &screenRect);

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
						else if( e.type == SDL_KEYDOWN )
						{
							Audio::MidiEvent midiEvent;
							midiEvent.eventCode = Audio::MIDI_NOTE_ON << 4;
							midiEvent.param1 = e.key.keysym.sym;
							Audio::midiController.ProcessEvent(midiEvent);
						}
						else if( e.type == SDL_KEYUP )
						{
							Audio::MidiEvent midiEvent;
							midiEvent.eventCode = Audio::MIDI_NOTE_OFF << 4;
							midiEvent.param1 = e.key.keysym.sym;
							Audio::midiController.ProcessEvent(midiEvent);
						}
					}
					
					//Apply the image
					SDL_BlitScaled( gHelloWorld, &imgRect, gScreenSurface, &screenRect );
					//Update the surface
					SDL_UpdateWindowSurface( gWindow );
				}
			}
			else
			{
				Debug::console(Debug::AUDIO, "Failed to init proper audio device\n");
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
        }
    }

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load splash image
    gHelloWorld = SDL_LoadBMP( "data/cat.bmp" );
    if( gHelloWorld == NULL )
    {
        Debug::console("Unable to load image %s! SDL Error: %s\n", "data/cat.bmp", SDL_GetError() );
        success = false;
    }

    return success;
}

void close()
{
	SDL_CloseAudio();

    //Deallocate surface
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}