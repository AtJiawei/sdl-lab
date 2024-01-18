#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "./constants.h"
#include "main.h"

typedef struct RenderingContext
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} RenderingContext;

// This initializes the SDL window
bool initialize_rendering_context(RenderingContext *context)
{
    *context = (RenderingContext){
        .window = NULL,
        .renderer = NULL,
    };

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    context->window = SDL_CreateWindow(
        "Pong game using C & SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (!context->window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!context->renderer)
    {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }

    return true;
}

void deinitialize_rendering_context(RenderingContext *context)
{
    if (context->renderer != NULL)
    {
        SDL_DestroyRenderer(context->renderer);
        context->renderer = NULL;
    }
    if (context->window != NULL)
    {
        SDL_DestroyWindow(context->window);
        context->window = NULL;
    }
    SDL_Quit();
}

// Poll events and process keyboard and mouse input
void process_input(World *world, bool *should_keep_running, Uint64 until)
{
    int up = 0;
    int down = 0;

    while (1)
    {
        SDL_Event event;
        SDL_PumpEvents();
        int peep = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
        if (peep < 0)
        {
            fprintf(stderr, "Failed to peek event");
            *should_keep_running = false;
            return;
        }
        // Stop if there are no more events.
        if (peep == 0)
        {
            break;
        }
        // Stop if the event occurred after the maximum simulated time.
        if ((event.type == SDL_QUIT && event.quit.timestamp > until) ||
            (event.type == SDL_KEYDOWN && event.key.timestamp > until))
        {
            break;
        }

        if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 0)
        {
            fprintf(stderr, "Unreachable code reached!\n");
            *should_keep_running = false;
            return;
        }

        switch (event.type)
        {
        case SDL_QUIT: // means when user clicks the x button to close the window
            *should_keep_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                *should_keep_running = false;
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

    // update left paddle speed
    world->paddle_left.vel.y = (down - up) * PADDLE_VEL;

    // process mouse input for right paddle
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    // determine velocity up and down based on mouse
    int paddle_right_middle_y = world->paddle_right.pos.y + world->paddle_right.dim.y / 2;
    float delta_y = mouse_y - paddle_right_middle_y;

    if (delta_y > 10)
    {
        // paddle goes down
        world->paddle_right.vel.y = PADDLE_VEL;
    }
    else if (delta_y < -10)
    {
        // paddle goes up
        world->paddle_right.vel.y = -PADDLE_VEL;
    }
    else
    {
        // paddle not move
        world->paddle_right.vel.y = 0;
    }
}

World create_world(void)
{
    return (World){
        .ball = create_ball(),
        .paddle_left = create_paddle_left(),
        .paddle_right = create_paddle_right(),
    };
}

GameObject create_paddle_right()
{
    return (GameObject){
        .pos = vector2f(
            WINDOW_WIDTH - PADDLE_INI_X - PADDLE_WIDTH,
            PADDLE_INI_Y),
        .dim = vector2f(
            PADDLE_WIDTH,
            PADDLE_HEIGHT),
        .vel = vector2f(0, 0),
    };
}

GameObject create_paddle_left()
{
    return (GameObject){
        .pos = vector2f(PADDLE_INI_X, PADDLE_INI_Y),
        .dim = vector2f(PADDLE_WIDTH, PADDLE_HEIGHT),
        .vel = vector2f(0, 0),
    };
}

// create a ball
GameObject create_ball()
{
    return (GameObject)
    {
        .pos = vector2f((WINDOW_WIDTH - BALL_WIDTH) / 2, (WINDOW_HEIGHT - BALL_HEIGHT) / 2),
        .dim = vector2f(BALL_WIDTH, BALL_HEIGHT),
        .vel = vector2f(BALL_VEL_X, BALL_VEL_Y),
    };
}
///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(World *world)
{
    float delta_time = (float)SIMULATION_TIME_STEP_MS / 1000.0;

    // Move ball as a function of delta time
    world->ball.pos.x += world->ball.vel.x * delta_time;
    world->ball.pos.y += world->ball.vel.y * delta_time;

    // Check for ball collision with the window up and bottom borders
    if (world->ball.pos.y <= 0)
    {
        world->ball.vel.y = -world->ball.vel.y;
    }
    if (world->ball.pos.y + world->ball.dim.y >= WINDOW_HEIGHT)
    {
        world->ball.vel.y = -world->ball.vel.y;
    }

    // Check for ball collision with the paddle_left
    if (world->ball.pos.x <= world->paddle_left.pos.x + world->paddle_left.dim.x)
    {
        if (world->ball.pos.y + world->ball.dim.y > world->paddle_left.pos.y && world->ball.pos.y < world->paddle_left.pos.y + world->paddle_left.dim.y)
        {
            world->ball.vel.x = -world->ball.vel.x;
        }

        if (world->ball.pos.x <= 0)
        {
            // TODO: add a pause scheme to start a new round
            world->ball = create_ball();
        }
    }
    if (world->ball.pos.x >= world->paddle_right.pos.x - world->ball.dim.x)
    {
        if (world->ball.pos.y + world->ball.dim.y > world->paddle_right.pos.y && world->ball.pos.y < world->paddle_right.pos.y + world->paddle_right.dim.y)
        {
            world->ball.vel.x = -world->ball.vel.x;
        }

        if (world->ball.pos.x >= WINDOW_WIDTH)
        {
            world->ball = create_ball();
        }
    }

    // update left paddle position
    world->paddle_left.pos.y += world->paddle_left.vel.y / SIMULATION_TIME_STEP_MS;

    // update right paddle position
    world->paddle_right.pos.y = world->paddle_right.pos.y + world->paddle_right.vel.y / SIMULATION_TIME_STEP_MS;

    // check collision of the right paddle to the border
    if (world->paddle_right.pos.y + world->paddle_right.dim.y >= WINDOW_HEIGHT)
    {
        world->paddle_right.pos.y = WINDOW_HEIGHT - world->paddle_right.dim.y;
    }
    if (world->paddle_right.pos.y <= 0)
    {
        world->paddle_right.pos.y = 0;
    }
    // check collision of the left paddle to the border
    if (world->paddle_left.pos.y + world->paddle_left.dim.y >= WINDOW_HEIGHT)
    {
        world->paddle_left.pos.y = WINDOW_HEIGHT - world->paddle_left.dim.y;
    }
    if (world->paddle_left.pos.y <= 0)
    {
        world->paddle_left.pos.y = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(RenderingContext *rendering_context, World *world)
{
    SDL_Renderer *renderer = rendering_context->renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R\G\B and alpha(transparency). 255 transparency = not transparent at all. It sets the draw color for the renderer to black with full alpha.
    SDL_RenderClear(renderer);                      // clears the entire rendering target with the color just activated. It essentially erases any previous content on the rendering target.

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)world->ball.pos.x,
        (int)world->ball.pos.y,
        (int)world->ball.dim.x,
        (int)world->ball.dim.y};                         // initiliazing the values using the corresponding properties of the 'ball' object.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a line in the middle of the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_left_rect = {
        (int)world->paddle_left.pos.x,
        (int)world->paddle_left.pos.y,
        (int)world->paddle_left.dim.x,
        (int)world->paddle_left.dim.y};

    // Draw a rectangle for the left paddle as paddle_left
    SDL_Rect paddle_right_rect = {
        (int)world->paddle_right.pos.x,
        (int)world->paddle_right.pos.y,
        (int)world->paddle_right.dim.x,
        (int)world->paddle_right.dim.y};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // sets the draw color
    SDL_RenderFillRect(renderer, &paddle_left_rect);
    SDL_RenderFillRect(renderer, &paddle_right_rect);

    SDL_RenderPresent(renderer); // double buffer. This is to present the hidden buffer to the screen / makes the updated rendering target visible to the
}

int main(int argc, char *args[])
{
    RenderingContext rendering_context;

    if (initialize_rendering_context(&rendering_context))
    {
        World world = create_world();

        Uint64 last_simulation_step_ticks = SDL_GetTicks64();

        bool should_keep_running = true;
        while (should_keep_running)
        {
            while (last_simulation_step_ticks + SIMULATION_TIME_STEP_MS < SDL_GetTicks64())
            {
                last_simulation_step_ticks += SIMULATION_TIME_STEP_MS;
                process_input(&world, &should_keep_running, last_simulation_step_ticks);
                update(&world);
            }
            render(&rendering_context, &world);
        }
    }

    deinitialize_rendering_context(&rendering_context);

    return 0;
}