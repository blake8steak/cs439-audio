#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>

/*

  ~ CS439 Final Project: game.cc ~

  This file contains the actual contents of the
  completed game.

*/


//  === GAME VARS ===
bool game_started = false;
bool in_studio = false;

//  === SYS VARS ===
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface*  bgSurface;
SDL_Texture* bgTexture;

/*
    ==== GAME METHODS ====
    the following methods are used for game mechanics.
    changing scenes, 
*/
void setBackground(std::string imgName) {
    std::string path = "images/" + imgName + ".png";
    bgSurface = IMG_Load(&path[0]);
    if (bgSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (bgTexture == nullptr) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
}

/*
    ==== SYSTEM METHODS ====
    the following methods are used to configure/interact 
    with PC hardware.
*/

int initGame() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // create window/renderer for the game
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

    return 0;
}

/*
    ~ main() ~
    the code that actually runs the game
*/

int main(int argc, char* argv[]) {

    initGame();
    setBackground("title");

    // set up menu loop
    bool is_running = true;
    while (is_running) {
        // clear screen
        SDL_RenderClear(renderer);

        // draw bg image
        SDL_Rect dest_rect;
        dest_rect.x = 0;
        dest_rect.y = 0;
        dest_rect.w = 800;
        dest_rect.h = 600;
        SDL_RenderCopy(renderer, bgTexture, NULL, &dest_rect);

        // handle keyboard input
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
                    setBackground("studio");
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
        }
    }
    // update screen
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }
  // clean up
  SDL_DestroyTexture(bgTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
