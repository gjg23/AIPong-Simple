#include "network.h"
#include "pong.h"
#include "train-data.h"

// Init pong class
PongGame::PongGame(PongNeuralNetwork* net): window(nullptr), renderer(nullptr), score(0), running(true), network(net)
{
    // Generate random seed
    std::srand(std::time(nullptr));

    // Init paddle
    paddle.x = 25;
    paddle.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    paddle.w = PADDLE_WIDTH;
    paddle.h = PADDLE_HEIGHT;

    // Init ball
    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.h = BALL_SIZE;
    ball.w = BALL_SIZE;


    // Setup ball speed
    reset_ball();

    // Start SDL
    init_SDL();
}

// Delete pong object
PongGame::~PongGame()
{
    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if(window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

// Reset the ball
void PongGame::reset_ball()
{
    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;

    // Randomize ball direction
    ball_speedx = BALL_SPEED;
    ball_speedy = BALL_SPEED;

    // Reset score
    score = 0;
}

// Init the SDL
void PongGame::init_SDL()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error initializing the SDL window:\n" << SDL_GetError() << std::endl;
        exit(1);
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error:\n" << TTF_GetError() << std::endl;
        exit(1);
    }

    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error:\n" << TTF_GetError() << std::endl;
        exit(1);
    }

    // Make window
    window = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window)
    {
        std::cerr << "Error making the window!:\n" << SDL_GetError() << std::endl;
        exit(1);
    }

    // Render window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        std::cerr << "Error making the renderer!:\n" << SDL_GetError() << std::endl;
        exit(1);
    }
}

// Event Handler
void PongGame::handle_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
    }
}

// Update
void PongGame::update_state(bool useai)
{
    // Move the ball
    ball.x += ball_speedx;
    ball.y += ball_speedy;

    // Check for collisions
    checkcollision();

    GameState stateinput({ball.y, paddle.y});

    int move = 2;
    if (useai)
    {
        move = network->predict_move(stateinput);
    }
    else
    {
        // Get the state of all keys
        const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

        // Check if the up arrow key is pressed
        if (currentKeyStates[SDL_SCANCODE_UP]) {
            move = 1;
        }

        // Check if the down arrow key is pressed
        if (currentKeyStates[SDL_SCANCODE_DOWN]) {
            move = 0;
        }
    }
    
    if (move == 1)
    {
        paddle.y = std::max(0, paddle.y - PADDLE_SPEED);
    }

    if (move == 0)
    {
        paddle.y = std::min(SCREEN_HEIGHT - PADDLE_HEIGHT, paddle.y + PADDLE_SPEED);
    }
}

// Check for colisions
void PongGame::checkcollision()
{
    // Top bottom walls
    if(ball.y <= 0 + ball.w || ball.y >= SCREEN_HEIGHT - ball.w)
    {
        ball_speedy = -ball_speedy;
    }

    // Right wall
    if(ball.x >= SCREEN_WIDTH - ball.w)
    {
        ball_speedx = -ball_speedx;
    }

    // left wall
    if(ball.x <= 0 + ball.w)
    {
        reset_ball();
    }

    // Paddle
    if(SDL_HasIntersection(&paddle, &ball))
    {
        ball_speedx = -ball_speedx;
        ball.x += ball_speedx;
        ball.y += ball_speedy;
        score++;
    }
}

void PongGame::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //set fill color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Render paddle and ball
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);

    // Render score
    renderscore();

    // Update screen
    SDL_RenderPresent(renderer);
}

void PongGame::renderscore()
{
    // Convert score to string
    std::string scoretext = "Score: " + std::to_string(score);

    // Create score surface
    SDL_Color textcolor = {255, 255, 255, 255}; // White
    SDL_Surface* textsurface = TTF_RenderText_Solid(font, scoretext.c_str(), textcolor);
    if(!textsurface)
    {
        std::cerr << "Unable to render text surface!\n" << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture* texttexture = SDL_CreateTextureFromSurface(renderer, textsurface);
    if(!texttexture)
    {
        std::cerr << "Unable to render text texture!\n" << SDL_GetError() << std::endl;
        return;
    }

    int textwidth = textsurface->w;
    int textheight = textsurface->h;

    SDL_Rect renderquad = {SCREEN_WIDTH - textwidth - 20, 20, textwidth, textheight};

    // Render text
    SDL_RenderCopy(renderer, texttexture, NULL, &renderquad);

    // Cleanup
    SDL_FreeSurface(textsurface);
    SDL_DestroyTexture(texttexture);
}

// Run
void PongGame::run(bool useai)
{
    // Main game loop
    while (running) {
        // Handle events
        handle_events();

        // Update game state
        update_state(useai);

        // Render game
        render();

        // Control frame rate
        SDL_Delay(16);  // Approximately 60 FPS
    }
}