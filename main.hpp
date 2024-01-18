#pragma once
#include "vector.cpp"

typedef struct GameObject
{
    // in pixels
    Vector2F pos;
    // in pixels
    Vector2F dim;
    // in pixels/simulation step
    Vector2F vel;
} GameObject;

GameObject game_object_default()
{
    return (GameObject){
        .pos = Vector2F(0, 0),
        // width = x, height = y;
        .dim = Vector2F(0, 0),
        .vel = Vector2F(0, 0),
    };
}

// what is a contruct?

typedef struct World
{
    GameObject ball;
    GameObject paddle_left;
    GameObject paddle_right;
} World;

GameObject create_ball();

GameObject create_paddle_left();

GameObject create_paddle_right();
