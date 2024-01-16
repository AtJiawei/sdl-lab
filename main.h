#pragma once

typedef struct GameObject
{
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} GameObject;

#define GAMEOBJECT_DEFAULT                                              \
    (GameObject)                                                        \
    {                                                                   \
        .x = 0, .y = 0, .width = 0, .height = 0, .vel_x = 0, .vel_y = 0 \
    }

typedef struct World
{
    GameObject ball;
    GameObject paddle_left;
    GameObject paddle_right;
} World;

GameObject create_ball();

GameObject create_paddle_left();

GameObject create_paddle_right();
