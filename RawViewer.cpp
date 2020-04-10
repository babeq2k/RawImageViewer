#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

using WndDtor = std::function<void(SDL_Window *)>;
using RenDtor = std::function<void(SDL_Renderer *)>;
using TxtDtor = std::function<void(SDL_Texture *)>;

class RawImageViewer {
public:
  RawImageViewer(int width, int height) : mWidth(width), mHeight(height) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      throw std::runtime_error("SDL_INIT error");
    }
    mWindow = std::unique_ptr<SDL_Window, WndDtor>(
        SDL_CreateWindow("RawImageViewer", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, width, height,
                         SDL_WINDOW_SHOWN),
        [](SDL_Window *wnd) {});
    if (mWindow == nullptr) {
      throw std::runtime_error("SDL_CreateWindow failure");
    }
    mRenderer = std::unique_ptr<SDL_Renderer, RenDtor>(
        SDL_CreateRenderer(mWindow.get(), -1,
                           SDL_RENDERER_ACCELERATED |
                               SDL_RENDERER_PRESENTVSYNC),
        [](SDL_Renderer *renderer) {});
    if (mRenderer == nullptr) {
      throw std::runtime_error("SDL_CreateRenderer error");
    }
    mTexture = std::unique_ptr<SDL_Texture, TxtDtor>(
      SDL_CreateTexture(mRenderer.get(), SDL_PIXELFORMAT_RGB24,
                          SDL_TEXTUREACCESS_STREAMING, width, height),
        [](SDL_Texture *texture) {});
  }

  bool eventHandle() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_Quit();
        return false;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          SDL_Quit();
          return false;
        }
      }
    }
    return true;
  }

  void render(const std::vector<char> &buffer) {
    SDL_Rect sourceRect {0, 0, mWidth, mHeight};
    SDL_Rect destRect {0, 0, mWidth, mHeight};
	  SDL_UpdateTexture(mTexture.get(), &sourceRect, buffer.data(), mWidth * 3);
	  SDL_RenderCopy(mRenderer.get(), mTexture.get(), &sourceRect, &destRect);
	  SDL_RenderPresent(mRenderer.get());
  }

private:
  int mWidth;
  int mHeight;
  std::unique_ptr<SDL_Window, WndDtor> mWindow;
  std::unique_ptr<SDL_Renderer, RenDtor> mRenderer;
  std::unique_ptr<SDL_Texture, TxtDtor> mTexture;
};

int main(int argc, const char **argv) {

  if (argc < 4) {
    throw std::runtime_error("usage: ./RawViewer file width height");
  }

  std::string fileName(*++argv);
  int width = std::stoi(*++argv);
  int height = std::stoi(*++argv);
  std::cout << fileName << ":" << width << "x" << height << std::endl;

  RawImageViewer viewer{width, height};

  std::vector<char> buf(width * height * 3, 0);
  std::ifstream ifs(fileName);
  if (ifs.good() == false) {
    throw std::runtime_error("failed to open file");
  }
  ifs.read(buf.data(), width * height * 3);
  ifs.close();
  viewer.render(buf);

  while (viewer.eventHandle())
    ;

  return 0;
}
