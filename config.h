#include <iostream>

// Global structures
#ifndef GLOBAL_H_
#define GLOBAL_H_

// Used to train the pong agent
struct TrainData
{
    // ball
    int bally;
    int paddley;
    int optimalmove;
};

// Used as live input for the agent
struct GameState
{
    // ball
    int bally;
    int paddley;
};

#endif


// Global settings
#ifndef PONG_CONFIG_H
#define PONG_CONFIG_H

// Screen
const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 800;

// Paddle
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 100;
const int PADDLE_SPEED = 7;

// Ball
const int BALL_SIZE = 10;
const int BALL_SPEED = 6;

#endif