#include "config.hpp"
#include <SDL_events.h>

constexpr int SCREEN_WIDTH{1024};
constexpr int SCREEN_HEIGHT{512};

struct AppState {
  SDL_Window *window{nullptr};
  SDL_Surface *surface{nullptr};
  SDL_Renderer *renderer{nullptr};
  SDL_Texture *texture{nullptr};
};

struct Player {
  float player_x = 3.456;
  float player_y = 2.456;
  float player_angle = 5.8;
  const float player_fov = M_PI / 3;
  int player_size = 5;
};

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (a << 24) | (b << 16) | (g << 8) | r;
}

void draw_rectangle(std::vector<uint32_t> &img, const size_t x, const size_t y,
                    const size_t w, const size_t h, const size_t color) {
  assert(img.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
  for (size_t i = 0; i < w; ++i)
    for (size_t j = 0; j < h; ++j) {
      int cx = x + i;
      int cy = y + j;

      if (cx >= SCREEN_WIDTH || cy >= SCREEN_HEIGHT)
        continue;
      img[cx + cy * SCREEN_WIDTH] = color;
    }
}

int32_t main(void) {
  AppState app;
  Player pl;

  int win_h = SCREEN_HEIGHT;
  int win_w = SCREEN_WIDTH;

  std::vector<uint32_t> framebuf(win_w * win_h, pack_color(255, 255, 255, 255));

  const size_t map_w = 16;
  const size_t map_h = 16;

  const char map[] = "0000222222220000"
                     "1    1         0"
                     "1         111110"
                     "1    1    2    0"
                     "0         2    0"
                     "011   2   1    0"
                     "0     1        0"
                     "0     1        0"
                     "0     1        0"
                     "0     1   111110"
                     "0              0"
                     "0              0"
                     "2       11111110"
                     "0              0"
                     "0              0"
                     "0002222222200000";

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL init video error!\n" << SDL_GetError();
  } else {
    app.window = SDL_CreateWindow("DOOD-engine", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

      // Draw map tiles
      size_t rect_w = SCREEN_WIDTH / (map_w * 2);
      size_t rect_h = SCREEN_HEIGHT / map_h;

      for (size_t my = 0; my < map_h; ++my) {
        for (size_t mx = 0; mx < map_w; ++mx) {
          if (map[my * map_w + mx] == ' ')
            continue;

          size_t rect_x = mx * rect_w;
          size_t rect_y = my * rect_h;

          draw_rectangle(framebuf, rect_x, rect_y, rect_w, rect_h,
                         pack_color(0, 255, 255, 255));
        }
      }

      draw_rectangle(framebuf, pl.player_x * rect_w, pl.player_y * rect_h,
                     pl.player_size, pl.player_size,
                     pack_color(255, 255, 255, 255));

      // Draw a field of view with first raycast
      for (int i = 0; i < win_w / 2; i++) {
        float angle =
            pl.player_angle - pl.player_fov / 2 +
            pl.player_fov * float(i) / float(static_cast<float>(win_w) / 2);

        for (float t = 0; t < 20; t += .05) {
          float cx = pl.player_x + t * cos(angle);
          float cy = pl.player_y + t * sin(angle);

          int map_x = int(cx);
          int map_y = int(cy);

          if (map_x < 0 || static_cast<int>(map_x) >= static_cast<int>(map_w) ||
              map_y < 0 || static_cast<int>(map_y) >= static_cast<int>(map_h))
            break;

          int px = int(cx * rect_w);
          int py = int(cy * rect_h);

          framebuf[px + py * win_w] =
              pack_color(160, 160, 160, 160); // this draws the visibility cone

          if (map[int(cx) + int(cy) * map_w] !=
              ' ') { // our ray touches a wall, so draw the vertical column to
            // create an illusion of 3D
            size_t column_height = win_h / t;

            draw_rectangle(framebuf, win_w / 2 + i,
                           win_h / 2 - column_height / 2, 1, column_height,
                           pack_color(255, 0, 255, 255));
            break;
          }
        }
      }

      // Copy to surface
      uint32_t *pixels = static_cast<uint32_t *>(app.surface->pixels);
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
