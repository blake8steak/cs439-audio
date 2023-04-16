#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>

const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 32767;
const double TWO_PI = 6.283185307179586476925286766559;

int main(int argc, char* argv[]) {

    // ==== parse audio data from file ====

    std::ifstream file("piano.wav", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file.wav" << std::endl;
        return 1;
    }

    // Read the header data from the file
    char header[44];
    if (!file.read(header, 44)) {
        std::cerr << "Failed to read header data" << std::endl;
        return 1;
    }

    // Get the size of the audio data
    const int dataSize = *((int*)(&header[40]));

    // Allocate a buffer to hold the audio data
    std::vector<char> buffer(dataSize);

    // Read the audio data from the file into the buffer
    if (!file.read(buffer.data(), dataSize)) {
        std::cerr << "Failed to read audio data" << std::endl;
        return 1;
    }

    // Close the .wav file
    file.close();

    std::cout << "Read " << dataSize << " bytes of audio data from file.wav" << std::endl;

    // ==== send audio data to speaker ====

    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 5000; //4096
    spec.callback = NULL;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    SDL_PauseAudioDevice(device, 0);

    //int arr[] = {80, 50, 10, 80, 50, 10, 80, 200, 100, 800, 90, 50, 5410, 80, 250, 10, 810, 2030, 1010, 800};
    const char* wavData = buffer.data();

    for(int k=0; k<2000; k++) {
        std::vector<Uint8> audioBuffer(spec.samples);

        auto x = wavData[k];
        std::cout << "read value: " << (int)x << std::endl;

        for (int i = 0; i < spec.samples; i++) {
            double t = (double)i / (double)spec.freq;
            double frequency = x;//0.01*k;
            double waveform = std::sin(TWO_PI * frequency * t);
            audioBuffer[i] = AMPLITUDE * waveform;
        }

        // queue audio data to play
        SDL_QueueAudio(device, audioBuffer.data(), audioBuffer.size());
    }

    // time to delay further action while queue clears
    SDL_Delay(2000);

    // close device
    SDL_CloseAudioDevice(device);
    SDL_Quit();

    return 0;
}