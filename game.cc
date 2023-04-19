#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <thread>

/*

  ~ CS439 Final Project: game.cc ~

  This file contains the actual contents of the
  completed game.

*/

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


//  === GAME VARS ===
const int TRACKLIST_SIZE = 6;
const int ALL_TRACKS_SIZE = 20;

bool game_started = false;
bool in_studio = true;
bool song_active = false;

struct SongData {
    std::string filename;
    std::string artist;
    std::string title;
    //AudioData data;
    int coolness_score;
    int royalty_cost;
    int sample_rate; // 44.1kHz or 48kHz
    int index;
};
int tracklist[] = {0, 1, 2, 3, 4, 5};
std::string static_sounds[] = {"static1", "static2", "static3"};
std::string ads[] = {"ozempic", "whopper", "homedepot"};
int ads_played = 0;
SongData complete_tracklist[ALL_TRACKS_SIZE];

int now_playing_index = tracklist[0];
int num_tracks_played = 0;
int money = 100;
int exp_money_increment = 15;


/*
    label stuff
*/
TTF_Font *prstartk;
SDL_Color COLOR_WHITE = {255, 255, 255};
SDL_Color COLOR_GREEN = {25, 255, 25};

char moneyBuff[100] = "$100";
SDL_Surface *money_surface;
SDL_Texture *money_texture;
SDL_Rect moneyDstRect = {617, 32, 150, 30};

char songTitleBuff[45] = "Title";
SDL_Surface *song_title_surface;
SDL_Texture *song_title_texture;
SDL_Rect songTitleDstRect = {235, 10, 325, 50};

/*
    ==== GAME METHODS ====
    the following methods are used for game mechanics.
    changing scenes, 
*/
void createSongArray() {
    /*
        std::string filename;
        std::string artist;
        std::string title;
        AudioData data; // NOT SURE IF NEEDED
        int sample_rate; // 44.1kHz or 48kHz
    */

    const int FOURTY_FOUR = 44100;
    const int FOURTY_EIGHT = 48000;
    std::string all_tracks[] = { "Psy-GangnamStyle", "NickiMinaj-SuperBass", "Nickelback-Animals", "KatyPerry-CaliforniaGurls", "TravisScott-SickoMode", "Umbrella-Rihanna", "TaylorSwift-22", "SouljaBoy-CrankThat", "SheckWes-MoBamba", "LMFAO-PartyRockAnthem", "LilUziVert-JustWannaRock", "KanyeWest-CantTellMeNothing", "TaylorSwift-WeAreNeverEverGettingBackTogether", "FooFighters-Everlong", "Drake-Passionfruit", "LilWayne-BillGates", "GreenDay-Holiday", "GreenDay-BasketCase", "GreenDay-AmericanIdiot", "GreenDay-BoulevardofBrokenDreams" };
    std::string all_artists[] = {"Psy", "Nicki Minaj", "Nickelback", "Katy Perry", "Travis Scott", "Rihanna", "Taylor Swift", "Soulja Boy", "Sheck Wes", "LMFAO", "Lil Uzi Vert", "Kanye West", "Taylor Swift", "Foo Fighters", "Drake", "Lil Wayne", "Green Day", "Green Day", "Green Day", "Green Day"};
    std::string all_titles[] = { "Gangnam Style", "Super Bass", "Animals", "California Gurls", "Sicko Mode", "Umbrella", "22", "Crank That",  "Mo Bamba", "Party Rock Anthem", "Just Wanna Rock", "Can\'t Tell Me Nothing", "We Are Never Ever", "Everlong", "Passionfruit", "Bill Gates", "Holiday", "Basket Case", "American Idiot", "Bvld of BD"};
    int all_sample_rates[] = { FOURTY_FOUR, FOURTY_EIGHT, FOURTY_FOUR, FOURTY_EIGHT, FOURTY_EIGHT, FOURTY_EIGHT, FOURTY_FOUR, FOURTY_EIGHT, FOURTY_EIGHT, FOURTY_FOUR, FOURTY_FOUR, FOURTY_EIGHT, FOURTY_EIGHT, FOURTY_FOUR, FOURTY_FOUR, FOURTY_EIGHT, FOURTY_EIGHT, FOURTY_FOUR, FOURTY_EIGHT, FOURTY_EIGHT};
    int all_coolness_scores[] = { 2, 12, 9, 8, 5, 14, 22, 1, 6, 2, 7, 8, 21, 24, 10, 10, 21, 26, 29, 21}; //cool
    int all_royalty_costs[] = { 1, 15, 4, 7, 3, 8, 22, 1, 3, 1, 3, 8, 14, 10, 5, 4, 12, 9, 30, 22 }; //royalty
    // create all SongData objects here
    for(int i=0; i<ALL_TRACKS_SIZE; i++) {
        complete_tracklist[i].filename = all_tracks[i];
        complete_tracklist[i].artist = all_artists[i];
        complete_tracklist[i].title = all_titles[i];
        complete_tracklist[i].sample_rate = all_sample_rates[i];
        complete_tracklist[i].coolness_score = all_coolness_scores[i];
        complete_tracklist[i].royalty_cost = all_royalty_costs[i];
        complete_tracklist[i].index = i;
    }
}

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

void hideSongLabel() {
    SDL_FreeSurface(song_title_surface);
    SDL_DestroyTexture(song_title_texture);
}

void hideMoneyLabel() {
    SDL_FreeSurface(money_surface);
    SDL_DestroyTexture(money_texture);
}

void genNewMoneyLabel() {
    for(int i=0; i<100; i++) {
        moneyBuff[i] = 0;
    }
    std::string monStr = "$"+std::to_string(money);
    for(int i=0; i<monStr.length(); i++) {
        moneyBuff[i] = monStr[i];
    }
    money_surface = TTF_RenderText_Solid(prstartk, moneyBuff, COLOR_GREEN);
    money_texture = SDL_CreateTextureFromSurface(renderer, money_surface);
}

void genNewSongLabels(std::string title, std::string artist) {
    for(int i=0; i<45; i++) {
        songTitleBuff[i] = 0;
    }
    for(int i=0; i<title.length(); i++) {
        songTitleBuff[i] = title[i];
    }
    songTitleBuff[title.length()] = ' ';
    songTitleBuff[title.length()+1] = '-';
    songTitleBuff[title.length()+2] = ' ';
    int maxIndex = title.length()+3+artist.length();
    for(int i=title.length()+3; i<maxIndex; i++) {
        std::cout << artist[i-(title.length()+3)];
        songTitleBuff[i] = artist[i-(title.length()+3)];
    }
    song_title_surface = TTF_RenderText_Solid(prstartk, songTitleBuff, COLOR_WHITE);
    song_title_texture = SDL_CreateTextureFromSurface(renderer, song_title_surface);
}

void genStudioLabels() {
    genNewMoneyLabel();
    genNewSongLabels(complete_tracklist[now_playing_index].title, complete_tracklist[now_playing_index].artist);
}

void hideStudioLabels() {
    hideMoneyLabel();
    hideSongLabel();
}

void playAudio(std::string audioName, int sampleRate, int royalty_cost) {
    std::string path = "sounds/" + audioName + ".wav";
    std::ifstream file(&path[0], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open audio file: " << audioName << " | sample rate " << sampleRate << std::endl;
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
    audioSpec.freq = sampleRate;

    //this is ugly but i'm lazy
    if(audioName == "whopper") {
        audioSpec.freq = 48000;
    }

    song_active = true;
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);

    if (deviceId == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudioDevice(deviceId, 0);
    // Wait for the audio to finish playing
    while (audio.position < audio.length && song_active) {
        SDL_Delay(100);
    }
    SDL_CloseAudioDevice(deviceId);
    if(audioName == "static1" || audioName == "static2" || audioName == "static3" || audioName == "shinobi3" || audioName == "whopper" || audioName == "homedepot" || audioName == "ozempic") {
        return;
    }

    num_tracks_played++;

    // decrease money
    money -= royalty_cost;
    if(in_studio)
            genNewMoneyLabel();
    std::cout << "-------- song played: paying $" << royalty_cost << " in royalties :( $" << money << std::endl;
    std::cout << "incremented num_tracks_played: " << num_tracks_played << std::endl;
    if(num_tracks_played % 2 == 0) {
        money += exp_money_increment;
        playAudio(static_sounds[ads_played%3], 44100, 0);
        playAudio(ads[ads_played%3], 44100, 0);
        ads_played++;
        std::cout << "+++++ Ad playing: money added! $" << money << std::endl;
        if(in_studio)
            genNewMoneyLabel();
    }
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
    //spec.freq = 44100;
    spec.freq = 48000;
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

    // init SDL TTF
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // init song data
    createSongArray();

    // init fonts, labels for text
    prstartk = TTF_OpenFont("fonts/prstartk.ttf", 24);
    money_surface = TTF_RenderText_Solid(prstartk, moneyBuff, COLOR_GREEN);
    money_texture = SDL_CreateTextureFromSurface(renderer, money_surface);

    return 0;
}

void playAudioWrapper() {
  playAudio("shinobi3", 44100, 0);
  while(true) {

    if(in_studio) {
        //playAudio(tracklist[num_tracks_played%TRACKLIST_SIZE]);
        now_playing_index = num_tracks_played%ALL_TRACKS_SIZE;
        //hideStudioLabels();
        genStudioLabels();
        SongData song = complete_tracklist[now_playing_index];
        std::cout << "**** now playing: " << song.title << " by "<< song.artist << " | sample rate: " << song.sample_rate << std::endl;
        
        playAudio(song.filename, song.sample_rate, song.royalty_cost);
        //std::cout << "**** now playing: " << tracklist[num_tracks_played%TRACKLIST_SIZE] << std::endl;
        std::string staticSound = "static" + std::to_string((num_tracks_played % 3)+1);
        playAudio(staticSound, 44100, 0);
    }
  }
}

/*
    ~ main() ~
    the code that actually runs the game
*/

int main(int argc, char* argv[]) {

    initGame();
    setBackground("title");
    std::thread audioThread(playAudioWrapper);

    hideMoneyLabel();

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
                        song_active = false;
                        std::cout << "Starting game..." << std::endl;
                        game_started = true;
                        setBackground("studio");
                        genStudioLabels();
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
                        song_active = false;
                        in_studio = false;
                        
                        std::cout << "change to see gheith..." << std::endl;
                        setBackground("gheith0");
                        hideStudioLabels();
                    }
                    break;
                case SDLK_x:
                    if(!in_studio) {
                        song_active = true;
                        in_studio = true;
                        std::cout << "returning to studio..." << std::endl;
                        setBackground("studio");
                        genStudioLabels();
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
    SDL_RenderCopy(renderer, song_title_texture, NULL, &songTitleDstRect);
    SDL_RenderCopy(renderer, money_texture, NULL, &moneyDstRect);
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
