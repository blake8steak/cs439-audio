#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

/*

  ~ CS439 Final Project: graphics.cc ~

  This file exists as a reference for how to:

  - open a window
  - create surfaces/textures
  - handle events
  - handle keyboard input

*/


SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface*  surface;
SDL_Texture* texture;

void goToStudio() {
    surface = IMG_Load("images/studio.png");
    if (surface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
}

int main(int argc, char* argv[]) {
  // Initialize SDL library
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create a window and a renderer for the menu screen
  window = SDL_CreateWindow("Bob-FM: The Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }
    renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == nullptr) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Load the menu background image
  surface = IMG_Load("images/title.png");
  if (surface == nullptr) {
    std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  if (texture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Set up the menu loop
  bool is_running = true;
  while (is_running) {
    // Clear the screen
    SDL_RenderClear(renderer);

    // Draw the menu background image
    SDL_Rect dest_rect;
    dest_rect.x = 0;
    dest_rect.y = 0;
    dest_rect.w = 800;
    dest_rect.h = 600;
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    // Handle user input
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_RETURN:
              std::cout << "Starting game..." << std::endl;
              goToStudio();
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

    // Update the screen
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  // Clean up resources
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
