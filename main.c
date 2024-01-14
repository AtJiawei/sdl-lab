#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "./constants.h"
#include "main.h"

#define PADDLE_WIDTH 7
#define PADDLE_HEIGHT 70
#define PADDLE_INI_X 20
#define PADDLE_INI_Y 250
#define STATIC 0

// speed in pixels/second
#define PADDLE_SPEED 900

#define BALL_WIDTH 5
#define BALL_HEIGHT 5
#define BALL_VEL_X -180
#define BALL_VEL_Y 60
///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
int last_frame_time = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object
{
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle_left, paddle_right;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        "Pong game using C & SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        SDL_Quit();
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard and mouse input
///////////////////////////////////////////////////////////////////////////////
void process_input(void)
{
    int up = 0;
    int down = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: // means when user clicks the x button to close the window
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                game_is_running = false;
                break;

            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                up = 1;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                down = 1;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                up = 0;
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                down = 0;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    // update paddle speed
    paddle_left.vel_y = 0;
    if (up && !down)
        paddle_left.vel_y = -PADDLE_SPEED;
    if (!up && down)
        paddle_left.vel_y = PADDLE_SPEED;

    // update paddle_left position
    paddle_left.y = paddle_left.y + paddle_left.vel_y / 60;

    // process mouse input
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    // determine velocity up and down based on mouse
    int paddle_right_middle_y = paddle_right.y + paddle_right.height / 2;
    float delta_y = mouse_y - paddle_right_middle_y;


    if (delta_y > 10)
    {
        // paddle goes down
        paddle_right.vel_y = PADDLE_SPEED;
    }
    else if (delta_y < -10)
    {
        // paddle goes up
        paddle_right.vel_y = - PADDLE_SPEED;
    }
    else
    {
        // paddle not move
        paddle_right.vel_y = 0;
    }

    // update paddle position
    paddle_right.y = paddle_right.y + paddle_right.vel_y / 60;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void)
{
    initialize_ball();

    initialize_paddle_left();

    initialize_paddle_right();
}

void initialize_paddle_right()
{
    // Initialize the paddle right's position
    paddle_right.x = WINDOW_WIDTH - PADDLE_INI_X - PADDLE_WIDTH;
    paddle_right.y = PADDLE_INI_Y;
    paddle_right.width = PADDLE_WIDTH;
    paddle_right.height = PADDLE_HEIGHT;
    paddle_right.vel_x = STATIC;
    paddle_right.vel_y = STATIC;
}

void initialize_paddle_left()
{
    // Initialize the paddle left's position
    paddle_left.x = PADDLE_INI_X;
    paddle_left.y = PADDLE_INI_Y;
    paddle_left.width = PADDLE_WIDTH;
    paddle_left.height = PADDLE_HEIGHT;
    paddle_left.vel_x = STATIC;
    paddle_left.vel_y = STATIC;
}

void initialize_ball()
{
    // Initialize the ball object moving down at a constant velocity
    ball.x = (WINDOW_WIDTH - ball.width) / 2;
    ball.y = (WINDOW_HEIGHT - ball.height) / 2;
    ball.width = BALL_WIDTH;
    ball.height = BALL_HEIGHT;
    ball.vel_x = BALL_VEL_X;
    ball.vel_y = BALL_VEL_Y;
}
///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void)
{
    // Get delta_time factor converted to seconds to be used to update objects
    float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next one
    last_frame_time = SDL_GetTicks64();

    // Move ball as a function of delta time
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // Check for ball collision with the window up and bottom borders
    if (ball.y <= 0)
    {
        ball.vel_y = -ball.vel_y;
    }
    if (ball.y + ball.height >= WINDOW_HEIGHT)
    {
        ball.vel_y = -ball.vel_y;
    }

    // Check for ball collision with the paddle_left
    if (ball.x <= paddle_left.x + paddle_left.width)
    {
        if (ball.y + ball.height > paddle_left.y && ball.y < paddle_left.y + paddle_left.height)
        {
            ball.vel_x = -ball.vel_x;
        }

        if (ball.x <= 0)
        {
            // TODO: add a pause scheme to start a new round
            initialize_ball();
        }
    }
    if (ball.x >= paddle_right.x - ball.width)
    {
        if (ball.y + ball.height > paddle_right.y && ball.y < paddle_right.y + paddle_right.height)
        {
            ball.vel_x = -ball.vel_x;
        }

        if (ball.x >= WINDOW_WIDTH)
        {
            initialize_ball();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R\G\B and alpha(transparency). 255 transparency = not transparent at all. It sets the draw color for the renderer to black with full alpha.
    SDL_RenderClear(renderer);                      // clears the entire rendering target with the color just activated. It essentially erases any previous content on the rendering target.

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height};                                // initiliazing the values using the corresponding properties of the 'ball' object.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a line in the middle of the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_left_rect = {
        (int)paddle_left.x,
        (int)paddle_left.y,
        (int)paddle_left.width,
        (int)paddle_left.height};

    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_right_rect = {
        (int)paddle_right.x,
        (int)paddle_right.y,
        (int)paddle_right.width,
        (int)paddle_right.height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &paddle_left_rect);
    SDL_RenderFillRect(renderer, &paddle_right_rect);

    SDL_RenderPresent(renderer); // double buffer. This is to present the hidden buffer to the screen / makes the updated rendering target visible to the
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *args[])
{
    game_is_running = initialize_window();

    setup();

    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}