#include "config.hpp"

constexpr int SCREEN_WIDTH{ 600 };
constexpr int SCREEN_HEIGHT{ 600 };

struct AppState
{
  SDL_Window* window{ nullptr };
  SDL_Surface* surface{ nullptr };
  SDL_Renderer* renderer{ nullptr };
  SDL_Texture* texture{ nullptr };
};

uint32_t
pack_color (uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  return (a << 24) | (b << 16) | (g << 8) | r;
}

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
                                     SCREEN_HEIGHT,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
      if (app.window == nullptr)
        {
          std::cout << "SDL cant create a window!\n" << SDL_GetError ();
        }
      else
        {
          app.renderer
              = SDL_CreateRenderer (app.window, -1, SDL_RENDERER_ACCELERATED);
          if (!app.renderer)
            {
              std::cout << "SDL renderer error!\n" << SDL_GetError ();
            }
          else
            SDL_RenderSetLogicalSize (app.renderer, SCREEN_WIDTH,
                                      SCREEN_HEIGHT);
          app.surface = SDL_CreateRGBSurfaceWithFormat (
              0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
          if (!app.surface)
            {
              std::cout << "SDL create RGB surface error!\n"
                        << SDL_GetError ();
            }
          SDL_LockSurface (app.surface);

          uint32_t* pixels = static_cast<uint32_t*> (app.surface->pixels);
          int pitch = app.surface->pitch;
          uint32_t pitch_in_pixels = pitch / 4;

          for (int j = 0; j < SCREEN_WIDTH; j++)
            {
              for (int i = 0; i < SCREEN_HEIGHT; i++)
                {
                  uint8_t r = static_cast<uint8_t> (255.0 * j / SCREEN_HEIGHT);
                  uint8_t g = static_cast<uint8_t> (255.0 * i / SCREEN_WIDTH);
                  uint8_t b = 0;
                  uint8_t a = 255;

                  pixels[j * pitch_in_pixels + i] = pack_color (r, g, b, a);
                }
            }

          SDL_UnlockSurface (app.surface);

          app.texture
              = SDL_CreateTextureFromSurface (app.renderer, app.surface);

          if (!app.texture)
            {
              std::cout << "SDL cant create texture from surface!\n"
                        << SDL_GetError ();
            }

          SDL_FreeSurface (app.surface);
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
          SDL_RenderClear (app.renderer);
          SDL_RenderCopy (app.renderer, app.texture, nullptr, nullptr);
          SDL_RenderPresent (app.renderer);
          SDL_Delay (16);
        }
    }

  SDL_DestroyTexture (app.texture);
  SDL_DestroyRenderer (app.renderer);
  SDL_DestroyWindow (app.window);
  SDL_Quit ();

  return 0;
}
