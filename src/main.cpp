#include "config.hpp"

struct AppState
{
  SDL_Window* window = nullptr;
  SDL_Surface* surface = nullptr;
};

int32_t
main (void)
{
  AppState app;

  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      std::cout << "SDL init video error!\n" << SDL_GetError ();
    }
  else
    {
      app.window = SDL_CreateWindow ("DOOD-engine", SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                     SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
      if (app.window == nullptr)
        {
          std::cout << "SDL cant create a window!\n" << SDL_GetError ();
        }
      else
        {
          app.surface = SDL_GetWindowSurface (app.window);
          SDL_FillRect (app.surface, nullptr,
                        SDL_MapRGB (app.surface->format, 0xFF, 0xFF, 0xFF));
          SDL_UpdateWindowSurface (app.window);
        }
      SDL_Event e;
      bool quit = false;
      while (quit == false)
        {
          while (SDL_PollEvent (&e))
            {
              if (e.type == SDL_QUIT)
                quit = true;
            }
        }
    }
  SDL_DestroyWindow (app.window);
  SDL_Quit ();

  return 0;
}
