#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Sprite {
  int32_t x{0};
  int32_t y{0};
  int32_t size{64};
  Sprite(int32_t x, int32_t y) : x(x), y(y) {}
};

constexpr uint32_t WIN_WIDTH = 1900;  // 1800;
constexpr uint32_t WIN_HEIGHT = 1200; // 1080;

constexpr int32_t LED_WIDTH = 64;
constexpr int32_t LED_HEIGHT = 64;
constexpr int32_t LED_SCALE = 8;
constexpr float PI = 3.14159265;
constexpr float AMPLITUDE = 180.0f;

SDL_Texture *loadSurface(SDL_Renderer *renderer, std::string path) {
  SDL_Texture *newTexture = NULL;

  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    return NULL;
  }
  // Load image at specified path
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
           IMG_GetError());
  } else {
    // Convert surface to screen format
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(),
             SDL_GetError());
    }

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}

void drawSinus(SDL_Renderer *renderer, SDL_Point *points) {
  // SDL_Point points[WIN_WIDTH];

  int yoffset = WIN_HEIGHT / 2;

  for (int index = WIN_WIDTH; index > 0; index--) {
    points[index].x = index;
    points[index].y = yoffset + AMPLITUDE * sin(index / 5 * PI / 180.0f);
  }
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0x00);
  SDL_RenderDrawPoints(renderer, points, WIN_WIDTH);
}

void createGrid(std::vector<Sprite> &sprites) {
  constexpr int32_t SPACING = 2;
  constexpr int32_t BORDER = 50;
  constexpr int32_t XAMOUNT =
      (WIN_WIDTH - BORDER * 2) / (LED_WIDTH / LED_SCALE + SPACING);
  constexpr int32_t YAMOUNT =
      (WIN_HEIGHT - BORDER * 2) / (LED_HEIGHT / LED_SCALE + SPACING);

  for (auto i = 0; i < XAMOUNT; i++) {
    for (auto n = 0; n < YAMOUNT; n++) {
      sprites.push_back(
          Sprite((LED_WIDTH / LED_SCALE) * i + i * SPACING + BORDER,
                 (LED_HEIGHT / LED_SCALE) * n + n * SPACING + BORDER));
    }
  }
}

int main() {

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("SDL2Test", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH,
                                        WIN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_Rect view;
  view.h = WIN_HEIGHT;
  view.w = WIN_WIDTH;
  view.x = 0;
  view.y = 0;
  if (SDL_RenderSetClipRect(renderer, &view) != 0) {
    return -1;
  }

  // SDL_Delay(3000);

  auto texture = loadSurface(renderer, "rectangle.png");

  SDL_Rect srcR;
  SDL_Rect destR;
  uint32_t textureFormat = 0;
  int access = 0;
  //  SDL_Point points[WIN_WIDTH];
  std::vector<Sprite> sprites;
  SDL_QueryTexture(texture, &textureFormat, &access, &srcR.w, &srcR.h);

  srcR.h = LED_HEIGHT;
  srcR.w = LED_WIDTH;
  destR.h = srcR.h / 1;
  destR.w = srcR.w / 1;

  if (texture == NULL) {
    goto end;
  }

  // clear scroll texture
  SDL_RenderClear(renderer);

  // build scroll line

  SDL_SetRenderTarget(renderer,
                      NULL); // reset back to default render target (screen)
  SDL_Rect dest;

  dest.x = 100;
  dest.y = 100;
  dest.w = LED_WIDTH / LED_SCALE;
  dest.h = LED_HEIGHT / LED_SCALE;

  createGrid(sprites);
  while (true) {

    // clear screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    // drawSinus(renderer, points);
    // Get the next event
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
        if (SDLK_ESCAPE == event.key.keysym.sym)
          goto end;
        break;
      }
      if (event.type == SDL_QUIT) {
        goto end;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
    for (auto sprite : sprites) {
      dest.x = sprite.x;
      dest.y = sprite.y;
      dest.w = sprite.size / LED_SCALE;
      dest.h = sprite.size / LED_SCALE;
      // std::cout << dest.x << " " << dest.y << std::endl;
      SDL_RenderCopy(renderer, texture, NULL, &dest);
    }
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
end:
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
