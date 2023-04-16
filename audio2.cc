#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>

const int BUFFER_SIZE = 4096;

struct AudioData {
  char* buffer;
  int length;
  int position;
};

void audioCallback(void* userdata, Uint8* stream, int len) {
  AudioData* audio = (AudioData*)userdata;

  if (audio->position == audio->length) {
    // If we've played all the audio, stop playing
    SDL_memset(stream, 0, len);
    return;
  }

  int remaining = audio->length - audio->position;
  int available = std::min(remaining, len);

  SDL_memcpy(stream, audio->buffer + audio->position, available);
  audio->position += available;
}

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    std::cerr << "Failed to initialize SDL audio: " << SDL_GetError() << std::endl;
    return 1;
  }

  std::ifstream file("drake_over.wav", std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open audio file." << std::endl;
    return 1;
  }

  file.seekg(0, std::ios::end);
  int length = file.tellg();
  file.seekg(0, std::ios::beg);

  char* audioData = new char[length];
  file.read(audioData, length);

  AudioData audio;
  audio.buffer = audioData;
  audio.length = length;
  audio.position = 0;

  SDL_AudioSpec audioSpec;
  audioSpec.freq = 44100;
  audioSpec.format = AUDIO_S16;
  audioSpec.channels = 2;
  audioSpec.samples = 4096;
  audioSpec.callback = audioCallback;
  audioSpec.userdata = &audio;

  SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
  if (deviceId == 0) {
    std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_PauseAudioDevice(deviceId, 0);

  // Wait for the audio to finish playing
  while (audio.position < audio.length) {
    SDL_Delay(100);
  }

  SDL_CloseAudioDevice(deviceId);
  SDL_Quit();

  delete[] audioData;

  return 0;
}