#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "./constants.h"
#include "main.h"

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
int last_frame_time = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

///////////////////////////////////////////////////////////////////////////////
// Declare game objects for the ball and the paddles
///////////////////////////////////////////////////////////////////////////////


// This initializes the SDL window
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

// Poll events and process keyboard and mouse input
void process_input(World *world)
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
    world->paddle_left.vel_y = 0;
    if (up && !down)
        world->paddle_left.vel_y = -PADDLE_SPEED;
    if (!up && down)
        world->paddle_left.vel_y = PADDLE_SPEED;

    // update paddle position
    world->paddle_left.y = world->paddle_left.y + world->paddle_left.vel_y / 60;

    // process mouse input
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    // determine velocity up and down based on mouse
    int paddle_right_middle_y = world->paddle_right.y + world->paddle_right.height / 2;
    float delta_y = mouse_y - paddle_right_middle_y;

    if (delta_y > 10)
    {
        // paddle goes down
        world->paddle_right.vel_y = PADDLE_SPEED;
    }
    else if (delta_y < -10)
    {
        // paddle goes up
        world->paddle_right.vel_y = -PADDLE_SPEED;
    }
    else
    {
        // paddle not move
        world->paddle_right.vel_y = 0;
    }
    // update paddle position
    world->paddle_right.y = world->paddle_right.y + world->paddle_right.vel_y / 60;

    // check collision of the right paddle to the border
    if (world->paddle_right.y + world->paddle_right.height >= WINDOW_HEIGHT)
    {
        world->paddle_right.y = WINDOW_HEIGHT - world->paddle_right.height;
    }
    if (world->paddle_right.y <= 0)
    {
        world->paddle_right.y = 0;
    }
    // check collision of the left paddle to the border
    if (world->paddle_left.y + world->paddle_left.height >= WINDOW_HEIGHT)
    {
        world->paddle_left.y = WINDOW_HEIGHT - world->paddle_left.height;
    }
    if (world->paddle_left.y <= 0)
    {
        world->paddle_left.y = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
World create_world(void)
{
    return (World){
        .ball = create_ball(),
        .paddle_left = create_paddle_right(),
        .paddle_right = create_paddle_left(),
    };
}

GameObject create_paddle_right()
{
    return (GameObject){

        .x = WINDOW_WIDTH - PADDLE_INI_X - PADDLE_WIDTH,
        .y = PADDLE_INI_Y,
        .width = PADDLE_WIDTH,
        .height = PADDLE_HEIGHT,
        .vel_x = STATIC,
        .vel_y = STATIC,
    };
}

GameObject create_paddle_left()
{
    return (GameObject){
        .x = PADDLE_INI_X,
        .y = PADDLE_INI_Y,
        .width = PADDLE_WIDTH,
        .height = PADDLE_HEIGHT,
        .vel_x = STATIC,
        .vel_y = STATIC,
    };
}

// create a ball
GameObject create_ball()
{
    return (GameObject){
        .x = (WINDOW_WIDTH - BALL_WIDTH) / 2,
        .y = (WINDOW_HEIGHT - BALL_HEIGHT) / 2,
        .width = BALL_WIDTH,
        .height = BALL_HEIGHT,
        .vel_x = BALL_VEL_X,
        .vel_y = BALL_VEL_Y};
}
///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(World *world)
{
    // Get delta_time factor converted to seconds to be used to update objects
    float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next one
    last_frame_time = SDL_GetTicks64();

    // Move ball as a function of delta time
    world->ball.x += world->ball.vel_x * delta_time;
    world->ball.y += world->ball.vel_y * delta_time;

    // Check for ball collision with the window up and bottom borders
    if (world->ball.y <= 0)
    {
        world->ball.vel_y = -world->ball.vel_y;
    }
    if (world->ball.y + world->ball.height >= WINDOW_HEIGHT)
    {
        world->ball.vel_y = -world->ball.vel_y;
    }

    // Check for ball collision with the paddle_left
    if (world->ball.x <= world->paddle_left.x + world->paddle_left.width)
    {
        if (world->ball.y + world->ball.height > world->paddle_left.y && world->ball.y < world->paddle_left.y + world->paddle_left.height)
        {
            world->ball.vel_x = -world->ball.vel_x;
        }

        if (world->ball.x <= 0)
        {
            // TODO: add a pause scheme to start a new round
            world->ball = create_ball();
        }
    }
    if (world->ball.x >= world->paddle_right.x - world->ball.width)
    {
        if (world->ball.y + world->ball.height > world->paddle_right.y && world->ball.y < world->paddle_right.y + world->paddle_right.height)
        {
            world->ball.vel_x = -world->ball.vel_x;
        }

        if (world->ball.x >= WINDOW_WIDTH)
        {
            world->ball = create_ball();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(World *world)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R\G\B and alpha(transparency). 255 transparency = not transparent at all. It sets the draw color for the renderer to black with full alpha.
    SDL_RenderClear(renderer);                      // clears the entire rendering target with the color just activated. It essentially erases any previous content on the rendering target.

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)world->ball.x,
        (int)world->ball.y,
        (int)world->ball.width,
        (int)world->ball.height};                          // initiliazing the values using the corresponding properties of the 'ball' object.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a line in the middle of the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_left_rect = {
        (int)world->paddle_left.x,
        (int)world->paddle_left.y,
        (int)world->paddle_left.width,
        (int)world->paddle_left.height};

    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_right_rect = {
        (int)world->paddle_right.x,
        (int)world->paddle_right.y,
        (int)world->paddle_right.width,
        (int)world->paddle_right.height};
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

    World world = create_world();

    while (game_is_running)
    {
        process_input(&world);
        update(&world);
        render(&world);
        SDL_Delay(1000 / 60);
    }

    destroy_window();

    return 0;
}