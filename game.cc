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

// -- using these to prevent double free() causing segfault
bool studioLabelsHidden = true;
bool tracklistLabelsHidden = true;
bool newSongLabelsHidden = true;

//  === GAME VARS ===
const int TRACKLIST_SIZE = 6;
const int ALL_TRACKS_SIZE = 20;

bool game_started = false;
bool in_studio = true;
bool song_active = false;
bool browsing_new_songs = false;
bool confirming_song_selection = false;
bool viewing_tracklist = false;
bool checking_gheith = false;

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

// used for new songs array
int randSongs[] = { 0, 0, 0 }; //rand()%ALL_TRACKS_SIZE, rand()%ALL_TRACKS_SIZE, rand()%ALL_TRACKS_SIZE

int tracklist[] = {4, 8, 9, 11, 14, 15};
std::string static_sounds[] = {"static1", "static2", "static3"};
std::string ads[] = { "ozempic", "homedepot", "whopper" };
int ads_played = 0;
SongData complete_tracklist[ALL_TRACKS_SIZE];
int now_playing_index = tracklist[0];
int num_tracks_played = 0;
int selected_new_song = 0;

/*
    game score/achievement level vars
*/
int money = 100;
int exp_money_increment = 15; //could vary depending on num active listeners
int active_listeners = 12;
int lifetime_coolness_points = 0;

/*
    label stuff
*/
TTF_Font *prstartk;
SDL_Color COLOR_WHITE = {255, 255, 255};
SDL_Color COLOR_GREEN = {25, 255, 25};
SDL_Color COLOR_DARK_GRAY = {50, 50, 50};

char moneyBuff[100] = "$100";
SDL_Surface *money_surface;
SDL_Texture *money_texture;
SDL_Rect moneyDstRect = {617, 32, 150, 30};

char songTitleBuff[45] = "Title";
SDL_Surface *song_title_surface;
SDL_Texture *song_title_texture;
SDL_Rect songTitleDstRect = {235, 10, 325, 50};

// new song select title label
char newSongSelectBuf[45] = "Title";
SDL_Surface *new_song_select_surface;
SDL_Texture *new_song_select_texture;
SDL_Rect newSongSelectDstRect = {230, 310, 345, 35};

// tracklist screen label stuff
char trackTitle1Buf[45] = "song1";
SDL_Surface *trackTitle1_surface;
SDL_Texture *trackTitle1_texture;
SDL_Rect trackTitle1DstRect = {50, 100, 120, 35};
char trackTitle2Buf[45] = "song2";
SDL_Surface *trackTitle2_surface;
SDL_Texture *trackTitle2_texture;
SDL_Rect trackTitle2DstRect = {50, 250, 120, 35};
char trackTitle3Buf[45] = "song3";
SDL_Surface *trackTitle3_surface;
SDL_Texture *trackTitle3_texture;
SDL_Rect trackTitle3DstRect = {50, 400, 120, 35};
char trackTitle4Buf[45] = "song4";
SDL_Surface *trackTitle4_surface;
SDL_Texture *trackTitle4_texture;
SDL_Rect trackTitle4DstRect = {550, 100, 120, 35};
char trackTitle5Buf[45] = "song5";
SDL_Surface *trackTitle5_surface;
SDL_Texture *trackTitle5_texture;
SDL_Rect trackTitle5DstRect = {550, 250, 120, 35};
char trackTitle6Buf[45] = "song6";
SDL_Surface *trackTitle6_surface;
SDL_Texture *trackTitle6_texture;
SDL_Rect trackTitle6DstRect = {550, 400, 120, 35};
//  -- tracklist subtitles
char trackSubtitle1Buf[45] = "blah info track1";
SDL_Surface *trackSubtitle1_surface;
SDL_Texture *trackSubtitle1_texture;
SDL_Rect trackSubtitle1DstRect = {50, 100, 120, 35};
char trackSubtitle2Buf[45] = "blah info track2";
SDL_Surface *trackSubtitle2_surface;
SDL_Texture *trackSubtitle2_texture;
SDL_Rect trackSubtitle2DstRect = {50, 250, 120, 35};
char trackSubtitle3Buf[45] = "blah info track3";
SDL_Surface *trackSubtitle3_surface;
SDL_Texture *trackSubtitle3_texture;
SDL_Rect trackSubtitle3DstRect = {50, 400, 120, 35};
char trackSubtitle4Buf[45] = "blah info track4";
SDL_Surface *trackSubtitle4_surface;
SDL_Texture *trackSubtitle4_texture;
SDL_Rect trackSubtitle4DstRect = {550, 100, 120, 35};
char trackSubtitle5Buf[45] = "blah info track5";
SDL_Surface *trackSubtitle5_surface;
SDL_Texture *trackSubtitle5_texture;
SDL_Rect trackSubtitle5DstRect = {550, 250, 120, 35};
char trackSubtitle6Buf[45] = "blah info track6";
SDL_Surface *trackSubtitle6_surface;
SDL_Texture *trackSubtitle6_texture;
SDL_Rect trackSubtitle6DstRect = {550, 400, 120, 35};
//      --- now, time for the selction screen
char newTitle1Buf[45] = "new1";
SDL_Surface *newTitle1_surface;
SDL_Texture *newTitle1_texture;
SDL_Rect newTitle1DstRect = {47, 315, 175, 50};
char newTitle2Buf[45] = "new2";
SDL_Surface *newTitle2_surface;
SDL_Texture *newTitle2_texture;
SDL_Rect newTitle2DstRect = {310, 315, 175, 50};
char newTitle3Buf[45] = "new3";
SDL_Surface *newTitle3_surface;
SDL_Texture *newTitle3_texture;
SDL_Rect newTitle3DstRect = {573, 315, 175, 50};
//             -- new subtitles
char newSubtitle1Buf[45] = "blah info new1";
SDL_Surface *newSubtitle1_surface;
SDL_Texture *newSubtitle1_texture;
SDL_Rect newSubtitle1DstRect = {33, 370, 200, 35};
char newSubtitle2Buf[45] = "blah info new2";
SDL_Surface *newSubtitle2_surface;
SDL_Texture *newSubtitle2_texture;
SDL_Rect newSubtitle2DstRect = {299, 370, 200, 35};
char newSubtitle3Buf[45] = "blah info new3";
SDL_Surface *newSubtitle3_surface;
SDL_Texture *newSubtitle3_texture;
SDL_Rect newSubtitle3DstRect = {560, 370, 200, 35};
/*
    ==== GAME METHODS ====
    the following methods are used for game mechanics.
    changing scenes, 
*/
void resizeRect(SDL_Rect &rect, int x, int y, int w, int h) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    //SDL_SetRect(&rect, x, y, w, h);
}

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
    std::string all_titles[] = { "Gangnam Style", "Super Bass", "Animals", "California Gurls", "Sicko Mode", "Umbrella", "   22   ", "Crank That",  "Mo Bamba", "Party Rock Anthem", "Just Wanna Rock", "Can\'t Tell Me Nothing", "We Are Never Ever", "Everlong", "Passionfruit", "Bill Gates", "Holiday", "Basket Case", "American Idiot", "Bvld of BD"};
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

void appendToBuffer(char buffer[], std::string stringToAdd, int startIndex) {
    int strIndex = 0;
    for(int i=startIndex; i<startIndex+stringToAdd.length(); i++) {
        buffer[i] = stringToAdd[strIndex];
        strIndex++;
    }
}

void showNewSongSelectLabel() {
    SongData selected = complete_tracklist[selected_new_song];
    for(int i=0; i<45; i++) {
        newSongSelectBuf[i] = 0;
    }
    appendToBuffer(newSongSelectBuf, selected.title, 0);
    appendToBuffer(newSongSelectBuf, " - ", selected.title.length());
    appendToBuffer(newSongSelectBuf, selected.artist, selected.title.length()+3);
    new_song_select_surface = TTF_RenderText_Solid(prstartk, newSongSelectBuf, COLOR_DARK_GRAY);
    new_song_select_texture = SDL_CreateTextureFromSurface(renderer, new_song_select_surface);
    resizeRect(newSongSelectDstRect, 230, 310, 345, 35);
}

void hideNewSongSelectLabel() {
    resizeRect(newSongSelectDstRect, 0, 0, 0, 0);
}

void showNewSongLabels() {
    newSongLabelsHidden = false;
    newTitle1_surface = TTF_RenderText_Solid(prstartk, newTitle1Buf, COLOR_DARK_GRAY);
    newTitle1_texture = SDL_CreateTextureFromSurface(renderer, newTitle1_surface);
    newTitle2_surface = TTF_RenderText_Solid(prstartk, newTitle2Buf, COLOR_DARK_GRAY);
    newTitle2_texture = SDL_CreateTextureFromSurface(renderer, newTitle2_surface);
    newTitle3_surface = TTF_RenderText_Solid(prstartk, newTitle3Buf, COLOR_DARK_GRAY);
    newTitle3_texture = SDL_CreateTextureFromSurface(renderer, newTitle3_surface);
    newSubtitle1_surface = TTF_RenderText_Solid(prstartk, newSubtitle1Buf, COLOR_DARK_GRAY);
    newSubtitle1_texture = SDL_CreateTextureFromSurface(renderer, newSubtitle1_surface);
    newSubtitle2_surface = TTF_RenderText_Solid(prstartk, newSubtitle2Buf, COLOR_DARK_GRAY);
    newSubtitle2_texture = SDL_CreateTextureFromSurface(renderer, newSubtitle2_surface);
    newSubtitle3_surface = TTF_RenderText_Solid(prstartk, newSubtitle3Buf, COLOR_DARK_GRAY);
    newSubtitle3_texture = SDL_CreateTextureFromSurface(renderer, newSubtitle3_surface);

    resizeRect(newTitle1DstRect, 47, 315, 175, 50);
    resizeRect(newTitle2DstRect, 310, 315, 175, 50);
    resizeRect(newTitle3DstRect, 573, 315, 175, 50);
    resizeRect(newSubtitle1DstRect, 33, 370, 200, 35);
    resizeRect(newSubtitle2DstRect, 299, 370, 200, 35);
    resizeRect(newSubtitle3DstRect, 560, 370, 200, 35);

    // newTitle1DstRect = {47, 315, 175, 50};
    // newTitle2DstRect = {310, 315, 175, 50};
    // newTitle3DstRect = {573, 315, 175, 50};
    // newSubtitle1DstRect = {33, 370, 200, 35};
    // newSubtitle2DstRect = {299, 370, 200, 35};
    // newSubtitle3DstRect = {560, 370, 200, 35};
}

void getNewSongsForLabels() {
    // probably create a bool here to toggle whether or not
    //    new songs should be selected or not...show() doesn't mean regen()!!
    srand((int) time(0));
    bool foundNewSongs = false;
    int numGood = 0;

    randSongs[0] = rand()%ALL_TRACKS_SIZE;
    randSongs[1] = rand()%ALL_TRACKS_SIZE;
    randSongs[2] = rand()%ALL_TRACKS_SIZE;

    while(!foundNewSongs) {
        bool isBad = false;
        for(int i=0; i<TRACKLIST_SIZE; i++) {
            // check if it's in tracklist
            if(tracklist[i] == randSongs[numGood]) {
                // not a new song
                randSongs[numGood] = rand()%ALL_TRACKS_SIZE;
                isBad = true;
            }
        }
        for(int j=0; j<3; j++) {
            //check if it's already in randSongs
            if(j == numGood) {
                continue;
            } else if(randSongs[j] == randSongs[numGood]) {
                randSongs[numGood] = rand()%ALL_TRACKS_SIZE;
                isBad = true;
                break;
            }
        }
        if(!isBad)
            numGood++;
        if(numGood == 3) {
            // found 3 new songs that don't exist in tracklist
            foundNewSongs = true;
        }
    }

    // wipe buffers
    const int bufLength = 45; //default buffer length I set for both title/subtitle
    for(int i=0; i<bufLength; i++) {
        newTitle1Buf[i] = 0;
        newTitle2Buf[i] = 0;
        newTitle3Buf[i] = 0;
        newSubtitle1Buf[i] = 0;
        newSubtitle2Buf[i] = 0;
        newSubtitle3Buf[i] = 0;
    }

    // populate title buffers
    SongData newSong1 = complete_tracklist[randSongs[0]];
    SongData newSong2 = complete_tracklist[randSongs[1]];
    SongData newSong3 = complete_tracklist[randSongs[2]];
    
    for(int i=0; i<newSong1.title.length(); i++) {
        newTitle1Buf[i] = newSong1.title[i];
    }
    for(int i=0; i<newSong2.title.length(); i++) {
        newTitle2Buf[i] = newSong2.title[i];
    }
    for(int i=0; i<newSong3.title.length(); i++) {
        newTitle3Buf[i] = newSong3.title[i];
    }

    // populate subtitle buffers
    std::string song1Artist = newSong1.artist;
    std::string song2Artist = newSong2.artist;
    std::string song3Artist = newSong3.artist;
    for(int i=0; i<song1Artist.length(); i++) {
        newSubtitle1Buf[i] = song1Artist[i];
    }
    for(int i=0; i<song2Artist.length(); i++) {
        newSubtitle2Buf[i] = song2Artist[i];
    }
    for(int i=0; i<song3Artist.length(); i++) {
        newSubtitle3Buf[i] = song3Artist[i];
    }
    std::string fourty_four = " | 44.1kHz";
    std::string fourty_eight = " | 48kHz";

    if(newSong1.sample_rate == 44100) {
        appendToBuffer(newSubtitle1Buf, fourty_four, song1Artist.length());
    } else {
        appendToBuffer(newSubtitle1Buf, fourty_eight, song1Artist.length());
    }
    if(newSong2.sample_rate == 44100) {
        appendToBuffer(newSubtitle2Buf, fourty_four, song2Artist.length());
    } else {
        appendToBuffer(newSubtitle2Buf, fourty_eight, song2Artist.length());
    }
    if(newSong3.sample_rate == 44100) {
        appendToBuffer(newSubtitle3Buf, fourty_four, song3Artist.length());
    } else {
        appendToBuffer(newSubtitle3Buf, fourty_eight, song3Artist.length());
    }

    // remember to render the labels after calling this!
    //     renderNewSongLabels();
}

void hideNewSongLabels() {
    if(!newSongLabelsHidden) {
        newSongLabelsHidden = true;
        SDL_FreeSurface(newTitle1_surface);
        SDL_DestroyTexture(newTitle1_texture);
        SDL_FreeSurface(newTitle2_surface);
        SDL_DestroyTexture(newTitle2_texture);
        SDL_FreeSurface(newTitle3_surface);
        SDL_DestroyTexture(newTitle3_texture);
        SDL_FreeSurface(newSubtitle1_surface);
        SDL_DestroyTexture(newSubtitle1_texture);
        SDL_FreeSurface(newSubtitle2_surface);
        SDL_DestroyTexture(newSubtitle2_texture);
        SDL_FreeSurface(newSubtitle3_surface);
        SDL_DestroyTexture(newSubtitle3_texture);
        resizeRect(newTitle1DstRect, 0, 0, 0, 0);
        resizeRect(newTitle2DstRect, 0, 0, 0, 0);
        resizeRect(newTitle3DstRect, 0, 0, 0, 0);
        resizeRect(newSubtitle1DstRect, 0, 0, 0, 0);
        resizeRect(newSubtitle2DstRect, 0, 0, 0, 0);
        resizeRect(newSubtitle3DstRect, 0, 0, 0, 0);
    }
}

void hideTracklistLabels() {
    if(!tracklistLabelsHidden) {
        tracklistLabelsHidden = true;
        SDL_FreeSurface(trackTitle1_surface);
        SDL_DestroyTexture(trackTitle1_texture);
        SDL_FreeSurface(trackTitle2_surface);
        SDL_DestroyTexture(trackTitle2_texture);
        SDL_FreeSurface(trackTitle3_surface);
        SDL_DestroyTexture(trackTitle3_texture);
        SDL_FreeSurface(trackTitle4_surface);
        SDL_DestroyTexture(trackTitle4_texture);
        SDL_FreeSurface(trackTitle5_surface);
        SDL_DestroyTexture(trackTitle5_texture);
        SDL_FreeSurface(trackTitle6_surface);
        SDL_DestroyTexture(trackTitle6_texture);
        SDL_FreeSurface(trackSubtitle1_surface);
        SDL_DestroyTexture(trackSubtitle1_texture);
        SDL_FreeSurface(trackSubtitle2_surface);
        SDL_DestroyTexture(trackSubtitle2_texture);
        SDL_FreeSurface(trackSubtitle3_surface);
        SDL_DestroyTexture(trackSubtitle3_texture);
        SDL_FreeSurface(trackSubtitle4_surface);
        SDL_DestroyTexture(trackSubtitle4_texture);
        SDL_FreeSurface(trackSubtitle5_surface);
        SDL_DestroyTexture(trackSubtitle5_texture);
        SDL_FreeSurface(trackSubtitle6_surface);
        SDL_DestroyTexture(trackSubtitle6_texture);
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
    genNewSongLabels(complete_tracklist[tracklist[now_playing_index]].title, complete_tracklist[tracklist[now_playing_index]].artist);
    // resizeRect(moneyDstRect, 617, 32, 150, 30);
    // resizeRect(songTitleDstRect, 235, 10, 325, 50);
    resizeRect(moneyDstRect, 617, 32, 150, 30);
    resizeRect(songTitleDstRect, 235, 10, 325, 50);
    // moneyDstRect = {617, 32, 150, 30};
    // songTitleDstRect = {235, 10, 325, 50};
    studioLabelsHidden = false;
}

void hideStudioLabels() {
    if(!studioLabelsHidden) {
        studioLabelsHidden = true;
        hideMoneyLabel();
        hideSongLabel();
        resizeRect(moneyDstRect, 0, 0, 0, 0);
        resizeRect(songTitleDstRect, 0, 0, 0, 0);
    }
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
        genStudioLabels();
    //std::cout << "-------- song played: paying $" << royalty_cost << " in royalties :( $" << money << std::endl;
    std::cout << "incremented num_tracks_played: " << num_tracks_played << std::endl;
    if(num_tracks_played % 3 == 0)
        getNewSongsForLabels();
    if(num_tracks_played % 2 == 0 && in_studio) {
        money += exp_money_increment;
        // do you want to be able to spam 'g''x''g''x' and make bank??
        // if so, remove the in_studio bool from the above conditional
        playAudio(static_sounds[num_tracks_played%3], 44100, 0);
        std::string adArtistText;
        if(ads[ads_played%3] == "whopper") {
            adArtistText = "Whopper";
        } else if (ads[ads_played%3] == "homedepot") {
            adArtistText = "Home Depot";
        } else {
            adArtistText = "Ozempic";
        }
        genNewSongLabels("$$ AD", adArtistText+" $$");
        playAudio(ads[ads_played%3], 44100, 0);
        ads_played++;
        std::cout << "+++++ Ad playing: money added! $" << money << " | New total_ads_played: " << ads_played << std::endl;
        if(in_studio) //leaving this in seems to prevent spam-scenarios where money label might still be active
            genStudioLabels();
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

    // get some random songs in new
    getNewSongsForLabels();

    return 0;
}

void playAudioWrapper() {
  playAudio("shinobi3", 44100, 0);
  while(true) {

    if(in_studio) {
        now_playing_index = num_tracks_played%TRACKLIST_SIZE;
        genStudioLabels();
        //SongData song = complete_tracklist[now_playing_index];
        SongData song = complete_tracklist[tracklist[now_playing_index]];
        std::cout << "**** now playing: " << song.title << " by "<< song.artist << " | sample rate: " << song.sample_rate << std::endl;   
        playAudio(song.filename, song.sample_rate, song.royalty_cost);
        playAudio(static_sounds[num_tracks_played%3], 44100, 0);
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
                case SDLK_v:
                    if(in_studio && !checking_gheith && !browsing_new_songs && !viewing_tracklist) {
                        in_studio = false;
                        viewing_tracklist = true;
                        std::cout << "change to view tracklist..." << std::endl;
                        setBackground("viewTracks");
                        hideStudioLabels();
                    }
                    break;
                case SDLK_n:
                    if(confirming_song_selection) {
                        setBackground("newSongs");
                        showNewSongLabels();
                        confirming_song_selection = false;
                        hideNewSongSelectLabel();
                    }
                    if(in_studio && !checking_gheith && !browsing_new_songs && !viewing_tracklist) {
                        in_studio = false;
                        browsing_new_songs = true;
                        std::cout << "change to new songs..." << std::endl;
                        setBackground("newSongs");
                        showNewSongLabels();
                        hideStudioLabels();
                    }
                    break;
                case SDLK_g:
                    if(in_studio && !checking_gheith && !browsing_new_songs && !viewing_tracklist) {
                        song_active = false;
                        in_studio = false;
                        checking_gheith = true;
                        std::cout << "change to see gheith..." << std::endl;
                        setBackground("gheith0");
                        hideStudioLabels();
                    }
                    break;
                case SDLK_x:
                    if(confirming_song_selection) {
                        break;
                    }
                    if(!in_studio) {
                        song_active = true;
                        in_studio = true;
                        std::cout << "returning to studio..." << std::endl;
                        hideNewSongLabels();
                        setBackground("studio");
                        genStudioLabels();
                        hideTracklistLabels();
                        hideNewSongLabels();
                        if(checking_gheith)
                            checking_gheith = false;
                        if(browsing_new_songs)
                            browsing_new_songs = false;
                        if(viewing_tracklist)
                            viewing_tracklist = false;
                    }
                    break;
                case SDLK_1:
                    if(browsing_new_songs) {
                        selected_new_song = randSongs[0];
                        std::cout << "selected option 1: " << complete_tracklist[selected_new_song].title << std::endl;
                        setBackground("confirmAdd");
                        hideNewSongLabels();
                        confirming_song_selection = true;
                        showNewSongSelectLabel();
                    }
                    break;
                case SDLK_2:
                    if(browsing_new_songs) {
                        selected_new_song = randSongs[1];
                        std::cout << "selected option 2: " << complete_tracklist[selected_new_song].title << std::endl;
                        setBackground("confirmAdd");
                        hideNewSongLabels();
                        confirming_song_selection = true;
                        showNewSongSelectLabel();
                    }
                    break;
                case SDLK_3:
                    if(browsing_new_songs) {
                        selected_new_song = randSongs[2];
                        std::cout << "selected option 3: " << complete_tracklist[selected_new_song].title << std::endl;
                        setBackground("confirmAdd");
                        hideNewSongLabels();
                        confirming_song_selection = true;
                        showNewSongSelectLabel();
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
    SDL_RenderCopy(renderer, new_song_select_texture, NULL, &newSongSelectDstRect);
    SDL_RenderCopy(renderer, newTitle1_texture, NULL, &newTitle1DstRect);
    SDL_RenderCopy(renderer, newTitle2_texture, NULL, &newTitle2DstRect);
    SDL_RenderCopy(renderer, newTitle3_texture, NULL, &newTitle3DstRect);
    SDL_RenderCopy(renderer, newSubtitle1_texture, NULL, &newSubtitle1DstRect);
    SDL_RenderCopy(renderer, newSubtitle2_texture, NULL, &newSubtitle2DstRect);
    SDL_RenderCopy(renderer, newSubtitle3_texture, NULL, &newSubtitle3DstRect);
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
