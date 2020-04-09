#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 
#include <iostream>

int main(int argc, const char **argv) {

  if (argc < 4) {
    throw std::runtime_error("usage: ./RawViewer file width height");
  }

  std::string fileName(*++argv);
  int width = std::stoi(*++argv);
  int height = std::stoi(*++argv);
  std::cout << fileName << ":" << width << "x" << height << std::endl;

  if (SDL_Init(SDL_INIT_VIDEO)) {
    throw std::runtime_error("SDL_INIT error");
  }
  SDL_Window *window = SDL_CreateWindow("RawViewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    throw std::runtime_error("SDL_CreateWindow failure");
  }

  bool IsExit{false};
  SDL_Event event;
  while (!IsExit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
				IsExit = true;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          IsExit = true;
      }
    }
  }

  SDL_Quit();
  return 0; 
}
