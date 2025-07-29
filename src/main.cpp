#include "config.hpp"

constexpr int SCREEN_WIDTH{ 592 };
constexpr int SCREEN_HEIGHT{ 600 };

const size_t win_w = SCREEN_WIDTH;
const size_t win_h = SCREEN_HEIGHT;

std::vector<uint32_t>
framebuf(SCREEN_WIDTH* SCREEN_HEIGHT, 255);

struct AppState
{
  SDL_Window* window{ nullptr };
  SDL_Surface* surface{ nullptr };
  SDL_Renderer* renderer{ nullptr };
  SDL_Texture* texture{ nullptr };
};

uint32_t
pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  return (a << 24) | (b << 16) | (g << 8) | r;
}

void
draw_triangel(std::vector<uint32_t>& img,
              const size_t x,
              const size_t y,
              const size_t w,
              const size_t h,
              const size_t color)
{
  assert(img.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
  for (int i = 0; i < w; ++i)
    for (int j = 0; j < h; ++j) {
      int cx = x + i;
      int cy = y + j;
      // assert (cx < w && cy < h);
      if (cx >= SCREEN_WIDTH || cy >= SCREEN_HEIGHT)
        continue;
      img[cx + cy * SCREEN_WIDTH] = color;
    }
}

int32_t
main(void)
{
  AppState app;

  const size_t map_w = 16;
  const size_t map_h = 16;

  // TODO: - understand how that shit work
  //       - understand whats the fuck are you
  //       - fix map render
  //       - fix logic issues with reading map

  const char map[] = "0000222222220000"
                     "1              0"
                     "1     1   111110"
                     "1     1        0"
                     "0     1        0"
                     "011   1        0"
                     "0     1        0"
                     "0     1        0"
                     "0     1        0"
                     "0     1   111110"
                     "0              0"
                     "0              0"
                     "2111111111111110"
                     // "0              0"
                     // "0              0"
                     // "0              0"
                     "0002222222200000";

  // assert (sizeof (map) == map_w * map_h * 1);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL init video error!\n" << SDL_GetError();
  } else {
    app.window = SDL_CreateWindow("DOOD-engine",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH,
                                  SCREEN_HEIGHT,
                                  SDL_WINDOW_SHOWN);
    if (app.window == nullptr) {
      std::cout << "SDL cant create a window!\n" << SDL_GetError();
    } else {
      app.renderer =
        SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);
      if (!app.renderer) {
        std::cout << "SDL renderer error!\n" << SDL_GetError();
      } else
        SDL_RenderSetLogicalSize(app.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
      app.surface = SDL_CreateRGBSurfaceWithFormat(
        0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
      if (!app.surface) {
        std::cout << "SDL create RGB surface error!\n" << SDL_GetError();
      }
      SDL_LockSurface(app.surface);

      for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
          uint8_t r = static_cast<uint8_t>(255.0 * x / SCREEN_WIDTH);
          uint8_t g = static_cast<uint8_t>(255.0 * y / SCREEN_HEIGHT);
          uint8_t b = 0;
          uint8_t a = 255;

          framebuf[y * SCREEN_WIDTH + x] = pack_color(r, g, b, a);
        }
      }

      // Draw map tiles
      size_t rect_w = SCREEN_WIDTH / map_w;
      size_t rect_h = SCREEN_HEIGHT / map_h;

      for (int my = 0; my < map_h; ++my) {
        for (int mx = 0; mx < map_w; ++mx) {
          if (map[my * map_w + mx] == ' ')
            continue;

          size_t rect_x = mx * rect_w;
          size_t rect_y = my * rect_h;

          draw_triangel(framebuf,
                        rect_x,
                        rect_y,
                        rect_w,
                        rect_h,
                        pack_color(0, 255, 255, 255));
        }
      }

      // Copy to surface
      uint32_t* pixels = static_cast<uint32_t*>(app.surface->pixels);
      int pitch_in_pixels = app.surface->pitch / 4;

      for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
          pixels[y * pitch_in_pixels + x] = framebuf[y * SCREEN_WIDTH + x];
        }
      }

      SDL_UnlockSurface(app.surface);

      app.texture = SDL_CreateTextureFromSurface(app.renderer, app.surface);

      if (!app.texture) {
        std::cout << "SDL cant create texture from surface!\n"
                  << SDL_GetError();
      }

      SDL_FreeSurface(app.surface);
    }
    SDL_Event e;
    bool quit = false;
    while (quit == false) {
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
          quit = true;
      }
      SDL_RenderClear(app.renderer);
      SDL_RenderCopy(app.renderer, app.texture, nullptr, nullptr);
      SDL_RenderPresent(app.renderer);
      SDL_Delay(16);
    }
  }

  SDL_DestroyTexture(app.texture);
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit();

  return 0;
}
