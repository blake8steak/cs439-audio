#include <iostream>
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>

const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 32767;
const double TWO_PI = 6.283185307179586476925286766559;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 30000; //4096
    spec.callback = NULL;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    SDL_PauseAudioDevice(device, 0);

    std::vector<Uint8> audioBuffer(spec.samples);

    for (int i = 0; i < spec.samples; i++) {
        double t = (double)i / (double)spec.freq;
        double frequency = 0.1;
        double waveform = std::sin(TWO_PI * frequency * t);
        audioBuffer[i] = AMPLITUDE * waveform;
    }

    // queue audio data to play
    SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());
    SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());
    SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());
    SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());
    SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());

    // time to delay further action while queue clears
    SDL_Delay(5000);

    // close device
    SDL_CloseAudioDevice(device);
    SDL_Quit();

    return 0;
}