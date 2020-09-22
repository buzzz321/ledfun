#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <set>
#include <string>
#include <time.h>
#include <vector>

using namespace std;

struct Sprite {
  float x{0};
  float y{0};
  int32_t xOrig{0};
  int32_t yOrig{0};
  float xAdd{0.0};
  float yAdd{0.0};
  int32_t scale{1};
  int32_t size{64};
  bool visible{true};
  Sprite(int32_t x, int32_t y, int32_t scale)
      : x(x), y(y), xOrig(x), yOrig(y), scale(scale) {}
};

constexpr uint32_t WIN_WIDTH = 2560;  // 1800;
constexpr uint32_t WIN_HEIGHT = 1440; // 1080;

constexpr int32_t LED_WIDTH = 64;
constexpr int32_t LED_HEIGHT = 64;
constexpr int32_t LED_SCALE = 4;
constexpr float PI = 3.14159265;

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

void fillTexture(SDL_Renderer *renderer, SDL_Texture *texture) {
  SDL_SetRenderTarget(renderer, texture);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, NULL);
}

void createGrid(std::vector<Sprite> &sprites) {
  constexpr int32_t SPACING = 2;
  constexpr int32_t BORDER = 150;
  constexpr int32_t XAMOUNT =
      (WIN_WIDTH - BORDER * 2) / (LED_WIDTH / LED_SCALE + SPACING);
  constexpr int32_t YAMOUNT =
      (WIN_HEIGHT - BORDER * 2) / (LED_HEIGHT / LED_SCALE + SPACING);

  for (auto i = 0; i < XAMOUNT; i++) {
    for (auto n = 0; n < YAMOUNT; n++) {
      sprites.push_back(Sprite(
          (LED_WIDTH / LED_SCALE) * i + i * SPACING + BORDER,
          (LED_HEIGHT / LED_SCALE) * n + n * SPACING + BORDER, LED_SCALE));
    }
  }
}

void clamp_add(Sprite &sprite, int32_t amount, int32_t clamp) {
  sprite.scale += amount;
  if (abs(sprite.scale) > abs(clamp)) {
    sprite.scale = clamp;
  }
  if (sprite.scale != LED_SCALE) {
    printf("scale = %d\n", sprite.scale);
  }
}

void powerOffLeds(std::vector<Sprite> &sprites, std::set<int32_t> &ledOff,
                  uint32_t amount = 10) {

  auto range = sprites.size();
  while (ledOff.size() < amount) {
    auto index = rand() % (range - 1);
    ledOff.insert(index);
    sprites[index].visible = true;
    sprites[index].scale = LED_SCALE;

    if (sprites[index].yOrig < (int32_t)WIN_HEIGHT / 2) {
      sprites[index].y = rand() % (sprites[index].yOrig);
    } else {
      sprites[index].y = sprites[index].yOrig + rand() % (sprites[index].yOrig);
    }
    if (sprites[index].xOrig < (int32_t)WIN_WIDTH / 2) {
      sprites[index].x = rand() % (sprites[index].xOrig);
    } else {
      sprites[index].x = sprites[index].xOrig + rand() % (sprites[index].xOrig);
    }
    auto hyp = hypot(sprites[index].x - sprites[index].xOrig,
                     sprites[index].y - sprites[index].yOrig);
    sprites[index].xAdd = (fabs(sprites[index].x - sprites[index].xOrig)) / hyp;
    sprites[index].yAdd = (fabs(sprites[index].y - sprites[index].yOrig)) / hyp;
    //    printf(" hyp = %f xadd= %f yadd= %f \n", hyp, sprites[index].xAdd,
    //           sprites[index].yAdd);
  }
}

void moveLedToOrig(std::vector<Sprite> &sprites, std::set<int32_t> &offLeds) {
  auto it = offLeds.begin();
  while (it != offLeds.end()) {
    // copy the current iterator then increment it
    std::set<int32_t>::iterator current = it++;
    int32_t index = *current;

    if ((int32_t)sprites[index].y != sprites[index].yOrig) {
      if (sprites[index].yOrig < (int32_t)WIN_HEIGHT / 2) {
        sprites[index].y += sprites[index].yAdd;
      } else {
        sprites[index].y -= sprites[index].yAdd;
      }
    }
    if ((int32_t)sprites[index].x != sprites[index].xOrig) {
      if (sprites[index].xOrig < (int32_t)WIN_WIDTH / 2) {
        sprites[index].x += sprites[index].xAdd;
      } else {
        sprites[index].x -= sprites[index].xAdd;
      }
    }
    if ((int32_t)sprites[index].y == sprites[index].yOrig &&
        (int32_t)sprites[index].x == sprites[index].xOrig) {
      sprites[index].visible = true;
      // don't invalidate iterator it, because it is already
      // pointing to the next element
      offLeds.erase(current);
      // printf("removed led\n");
    }
  }
}

int main() {
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("SDL2Test", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH,
                                        WIN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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
  if (SDL_RenderSetClipRect(renderer, &view) != 0) { // set screen clipping
    return -1;
  }

  auto texture = loadSurface(renderer, "rectangle.png");
  auto texture2 = loadSurface(renderer, "rectangles.png");

  SDL_Rect srcR;
  SDL_Rect destR;
  uint32_t textureFormat = 0;
  int access = 0;
  timespec time1, time2, timerresolution;
  std::vector<Sprite> sprites;
  std::set<int32_t> offLeds;
  uint32_t skipper{0};

  SDL_QueryTexture(texture, &textureFormat, &access, &srcR.w, &srcR.h);

  auto blackTexture = SDL_CreateTexture(
      renderer, textureFormat, SDL_TEXTUREACCESS_STATIC, LED_WIDTH, LED_HEIGHT);

  if (blackTexture == NULL) {
    printf("Unable to create texture SDL Error: %s\n", SDL_GetError());
    goto endWithTexture;
  }

  fillTexture(renderer, blackTexture);

  srcR.h = LED_HEIGHT;
  srcR.w = LED_WIDTH;
  destR.h = srcR.h / 1;
  destR.w = srcR.w / 1;

  if (texture == NULL) {
    goto end;
  }

  SDL_Rect dest;

  dest.x = 100;
  dest.y = 100;
  dest.w = LED_WIDTH / LED_SCALE;
  dest.h = LED_HEIGHT / LED_SCALE;

  createGrid(sprites);
  clock_getres(CLOCK_PROCESS_CPUTIME_ID, &timerresolution);

  while (true) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    SDL_SetRenderTarget(renderer,
                        NULL); // reset back to default render target (screen)
    // clear screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer); // use above color to clear screen/renderer

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
    // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);

    for (auto sprite : sprites) {
      dest.x = sprite.x;
      dest.y = sprite.y;
      dest.w = sprite.size / sprite.scale;
      dest.h = sprite.size / sprite.scale;

      if (!sprite.visible) {
        SDL_RenderCopy(renderer, blackTexture, NULL, &dest);
      } else {
        // std::cout << dest.x << " " << dest.y << std::endl;
        SDL_RenderCopy(renderer, texture, NULL, &dest);
      }
      skipper++;
      if (skipper % 1024 == 0) {

        moveLedToOrig(sprites, offLeds);
        // to led_scale
        powerOffLeds(sprites, offLeds, 300);
      }
      if (skipper % 65535 * 4 == 0) {
      }
    }
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    //    std::cout << "resolution: " << timerresolution.tv_nsec << "
    //    timediff "
    //              << (time2.tv_nsec - time1.tv_nsec) / 1000 << std::endl;
  }

  SDL_DestroyTexture(blackTexture);
endWithTexture:
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(texture2);
  SDL_DestroyRenderer(renderer);

end:
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
