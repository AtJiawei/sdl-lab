#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_timer.h>

#include "./constants.h"

#define PADDLE_WIDTH 7
#define PADDLE_HEIGHT 70
#define PADDLE_INI_X 20
#define PADDLE_INI_Y 250
#define PADDLE_INI_V 0
#define PADDLE_VEL_X 0

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
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle1, paddle2;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n"); 
        return false;
    }
    window = SDL_CreateWindow(
        "Pong game using C & SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        SDL_Quit();
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) { 
        switch (event.type) {
            case SDL_QUIT: // means when user clicks the x button to close the window 
                game_is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) { // check which key is pressed. if Esc
                    game_is_running = false;
                }
                if(event.key.keysym.sym == SDLK_DOWN){
                    paddle1.y = paddle1.y + 10;
                }
                if(event.key.keysym.sym == SDLK_UP){
                    paddle1.y = paddle1.y - 10;
                }

                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // Initialize the ball object moving down at a constant velocity
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.width = 10;
    ball.height = 10;
    ball.vel_x = 180;
    ball.vel_y = 0;


    // Initialize the paddle 1 object's position
    paddle1.x = PADDLE_INI_X;
    paddle1.y = PADDLE_INI_Y;
    paddle1.width = PADDLE_WIDTH;
    paddle1.height = PADDLE_HEIGHT;
    paddle1.vel_x = PADDLE_VEL_X ;
    paddle1.vel_y = 0;

    // Initialize the paddle 2 object's position
    paddle2.x = WINDOW_WIDTH - PADDLE_INI_X - PADDLE_WIDTH;
    paddle2.y = PADDLE_INI_Y;
    paddle2.width = PADDLE_WIDTH;
    paddle2.height = PADDLE_HEIGHT;
    paddle2.vel_x = PADDLE_VEL_X;
    paddle2.vel_y = 0;

}

///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    // Get delta_time factor converted to seconds to be used to update objects
    float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next one
    last_frame_time = SDL_GetTicks64();

    // Move ball as a function of delta time
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // Check for ball collision with the window up and bottom borders
    if (ball.y <= 0) {
        ball.vel_y = -ball.vel_y;
    }
    if (ball.y + ball.height >= WINDOW_HEIGHT) {
        ball.vel_y = -ball.vel_y;
    }


    // Check for ball collision with the paddle1
    if (ball.x <= paddle1.x + paddle1.width){
        if (ball.y + ball.height > paddle1.y && ball.y < paddle1.y + paddle1.height)
        {
            SDL_Delay(1000);
            ball.vel_x = -ball.vel_x;
        }
        
        if (ball.x <= 0)
        {
            setup();
        }
    }
    if (ball.x >= paddle2.x - ball.width) {
        if (ball.y + ball.height > paddle2.y && ball.y < paddle2.y + paddle2.height)
        {
            ball.vel_x = -ball.vel_x;
        }

        if (ball.x >= WINDOW_WIDTH)
        {
            setup();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R\G\B and alpha(transparency). 255 transparency = not transparent at all. It sets the draw color for the renderer to black with full alpha. 
    SDL_RenderClear(renderer); // clears the entire rendering target with the color just activated. It essentially erases any previous content on the rendering target. 

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    }; // initiliazing the values using the corresponding properties of the 'ball' object.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &ball_rect);


    // Draw a line in the middle of the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT);


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Draw a rectangle for the left paddle as paddle1 
    SDL_Rect paddle1_rect = {
        (int)paddle1.x,
        (int)paddle1.y,
        (int)paddle1.width,
        (int)paddle1.height
    };

     // Draw a rectangle for the left paddle as paddle1 
    SDL_Rect paddle2_rect = {
        (int)paddle2.x,
        (int)paddle2.y,
        (int)paddle2.width,
        (int)paddle2.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &paddle1_rect);
    SDL_RenderFillRect(renderer, &paddle2_rect);


    SDL_RenderPresent(renderer); // double buffer. This is to present the hidden buffer to the screen / makes the updated rendering target visible to the
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[]) {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}