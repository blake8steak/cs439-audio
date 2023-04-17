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
bool in_studio = true;

/*  

    === SYS VARS ===

*/

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface*  bgSurface;
SDL_Texture* bgTexture;

const int BUFFER_SIZE = 4096; // audio buffer
SDL_AudioSpec audioSpec;
struct AudioData {
  char* buffer;
  int length;
  int position;
};
AudioData audioData;

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
void playAudio(std::string audioName) {
    std::string path = "sounds/" + audioName + ".wav";
    std::ifstream file(&path[0], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open audio file." << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    int length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* audioBuff = new char[length];
    file.read(audioBuff, length);

    AudioData audio;
    audio.buffer = audioBuff;
    audio.length = length;
    audio.position = 0;
    audioData = audio;

    audioSpec.userdata = &audio;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);

    if (deviceId == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudioDevice(deviceId, 0);
    // Wait for the audio to finish playing
    while (audio.position < audio.length) {
        SDL_Delay(100);
    }
    SDL_CloseAudioDevice(deviceId);
}

/*
    ==== SYSTEM METHODS ====
    the following methods are used to configure/interact 
    with PC hardware.
*/

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioData* audio = (AudioData*)userdata;

    if (audio->position == audio->length) {
        // if all audio has been played, stop
        SDL_memset(stream, 0, len);
        return;
    }

    int remaining = audio->length - audio->position;
    int available = std::min(remaining, len);

    SDL_memcpy(stream, audio->buffer + audio->position, available);
    audio->position += available;
}

int initGame() {
    // init SDL video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Video Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // init SDL audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Audio Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_AudioSpec spec;
    spec.freq = 44100;
    spec.format = AUDIO_S16;
    spec.channels = 2;
    spec.samples = 4096;
    spec.callback = audioCallback;
    // spec.userdata = &audio; // set this when audio data is received
    audioSpec = spec;

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
    playAudio("SEGA");

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
                    if(!game_started) {
                        std::cout << "Starting game..." << std::endl;
                        game_started = true;
                        setBackground("studio");
                    }
                    break;
                case SDLK_a:
                    if(in_studio) {
                        in_studio = false;
                        std::cout << "change to ads scene..." << std::endl;
                        setBackground("ads");
                    }
                    break;
                case SDLK_b:
                    if(in_studio) {
                        in_studio = false;
                        std::cout << "change to buy new songs..." << std::endl;
                        setBackground("buy");
                    }
                    break;
                case SDLK_c:
                    if(in_studio) {
                        in_studio = false;
                        std::cout << "change to check alicefm..." << std::endl;
                        setBackground("aliceFM");
                    }
                    break;
                case SDLK_e:
                    if(in_studio) {
                        in_studio = false;
                        std::cout << "change to edit setlist..." << std::endl;
                        setBackground("setlist");
                    }
                    break;
                case SDLK_g:
                    if(in_studio) {
                        in_studio = false;
                        std::cout << "change to see gheith..." << std::endl;
                        setBackground("gheith0");
                    }
                    break;
                case SDLK_x:
                    if(!in_studio) {
                        in_studio = true;
                        std::cout << "returning to studio..." << std::endl;
                        setBackground("studio");
                    }
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
