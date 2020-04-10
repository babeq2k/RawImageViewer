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
  RawImageViewer(const std::string &format, int width, int height) : mWidth(width), mHeight(height) {
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

    setConfig(format);
    mTexture = std::unique_ptr<SDL_Texture, TxtDtor>(
      SDL_CreateTexture(mRenderer.get(), mFormat,
                          SDL_TEXTUREACCESS_STREAMING, width, height),
        [](SDL_Texture *texture) {});
  }

  bool eventHandle() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN) {
        SDL_Quit();
        return false;
      }
    }
    return true;
  }

  void render(const std::vector<char> &buffer) {
    SDL_Rect sourceRect {0, 0, mWidth, mHeight};
    SDL_Rect destRect {0, 0, mWidth, mHeight};
	  SDL_UpdateTexture(mTexture.get(), &sourceRect, buffer.data(), mStride);
	  SDL_RenderCopy(mRenderer.get(), mTexture.get(), &sourceRect, &destRect);
	  SDL_RenderPresent(mRenderer.get());
  }

  int inputBufferSize() {
    if (mFormat == SDL_PIXELFORMAT_RGB24) {
      return mWidth * mHeight * 3;
    }
    if (mFormat == SDL_PIXELFORMAT_NV21 ||
        mFormat == SDL_PIXELFORMAT_NV12) {
      return mWidth * mHeight * 3 / 2;
    }
    return mWidth * mHeight * 3;
  }

private:
  void setConfig(const std::string &format) {
    if (format == "rgb24") {
      mFormat = SDL_PIXELFORMAT_RGB24;
      mStride = mWidth * 3;
    } else if (format == "nv21") {
      mFormat = SDL_PIXELFORMAT_NV21;
      mStride = mWidth;
    } else if (format == "nv12") {
      mFormat = SDL_PIXELFORMAT_NV12;
      mStride = mWidth;
    }
  }

  int mWidth;
  int mHeight;
  uint32_t mFormat;
  int mStride;
  std::unique_ptr<SDL_Window, WndDtor> mWindow;
  std::unique_ptr<SDL_Renderer, RenDtor> mRenderer;
  std::unique_ptr<SDL_Texture, TxtDtor> mTexture;

};

int main(int argc, const char **argv) {

  if (argc < 5) {
    throw std::runtime_error("usage: ./RawViewer images/red_768x720.rgb24 rgb24 width height");
  }
  std::string fileName(*++argv);
  std::string format(*++argv);
  int width = std::stoi(*++argv);
  int height = std::stoi(*++argv);

  RawImageViewer viewer{format, width, height};

  std::vector<char> buffer(viewer.inputBufferSize(), 0);
  std::ifstream ifs(fileName);
  if (ifs.good() == false) {
    throw std::runtime_error("failed to open file");
  }
  ifs.read(buffer.data(), buffer.size());
  ifs.close();
  viewer.render(buffer);

  while (viewer.eventHandle())
    ;

  return 0;
}
