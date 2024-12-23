#ifndef PONG
#define PONG

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "config.h"

class PongNeuralNetwork;

class PongStateGenerator;

class PongGame
{
private:
    // SDL
    SDL_Window*  window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    // Game objects;
    SDL_Rect paddle;
    SDL_Rect ball;

    // Game State
    int score;
    int ball_speedx;
    int ball_speedy;
    bool running;

    // Agent
    PongNeuralNetwork* network;

    // Helper methods
    void init_SDL();
    void handle_events();
    void update_state(bool useai);
    void render();
    void reset_ball();
    void checkcollision();
    void renderscore();
    int findhit(int x, int y, int dx, int dy);

public:
    PongGame(PongNeuralNetwork* net);
    ~PongGame();

    void run(bool useai);
};

#endif