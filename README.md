# Bob-FM: The Game
For our final CS439 project, my group (me and Trish Truong) created a game from scratch, using nothing but plain C++ and SDL. We called it Bob-FM: The Game; we really enjoyed working on it, and we were happy to see that people seemed to enjoy it when we presented.

To recompile the game, enter the following command:

g++ -o bob-fm game.cc \$(pkg-config --cflags --libs sdl2) \$(pkg-config --cflags --libs SDL2_image) \$(pkg-config --cflags --libs SDL2_ttf)

-----------
To recompile audio.cc, enter the following command into a terminal:
### g++ audio2.cc -o program2 -lSDL2

To recompile graphics.cc, enter the following command:
### g++ -o graphics graphics.cc $(pkg-config --cflags --libs sdl2) $(pkg-config --cflags --libs SDL2_image)
