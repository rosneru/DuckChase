/*
 * This source code copyrighted by Lazy Foo' Productions (2004-2015) 
 * and may not be redistributed without written permission
 */

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[])
{
  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 20;
  }

  //Create window
  SDL_Window* pWindow = SDL_CreateWindow("SDL Tutorial", 
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, 
    SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (pWindow == NULL)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 20;
  }

  //Get window surface
  SDL_Surface* pScreenSurface = SDL_GetWindowSurface(pWindow);

  //Fill the surface white
  SDL_Surface* pPictureSurface = SDL_LoadBMP("Images/Hello_world.bmp");
  if (pPictureSurface == NULL)
  {
    printf("Unable to load the image: %s\n", SDL_GetError());
    return 20;
  }

  SDL_Event e;
  bool bQuit = false;

  // Main loop
  while (bQuit == false)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
      {
        bQuit = true;
      }
    }

    // To prevent using 100% CPU time
    SDL_Delay(10);

    //Apply the image
    SDL_BlitSurface(pPictureSurface, NULL, pScreenSurface, NULL);

    //Update the surface
    SDL_UpdateWindowSurface(pWindow);
  }

  SDL_FreeSurface(pPictureSurface);

  //Destroy window
  SDL_DestroyWindow(pWindow);

  //Quit SDL subsystems
  SDL_Quit();

  return 0;
}
