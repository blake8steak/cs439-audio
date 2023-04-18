# cs439-audio
 Final project for CS439, a C program that can read/play audio files.

To recompile the game, enter the following command:

### g++ -o bob-fm game.cc $(pkg-config --cflags --libs sdl2) $(pkg-config --cflags --libs SDL2_image) $(pkg-config --cflags --libs SDL2_ttf)


-----------
To recompile audio.cc, enter the following command into a terminal:
### g++ audio2.cc -o program2 -lSDL2

To recompile graphics.cc, enter the following command:
### g++ -o graphics graphics.cc $(pkg-config --cflags --libs sdl2) $(pkg-config --cflags --libs SDL2_image)