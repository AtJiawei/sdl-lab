#pragma once

typedef struct Vector2F
{
    float x;
    float y;
} Vector2F;

Vector2F vector2f(float x, float y)
{
    return (Vector2F){
        .x = x,
        .y = y,
    };
}

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
        .pos = vector2f(0, 0),
        // width = x, height = y;
        .dim = vector2f(0, 0),
        .vel = vector2f(0, 0),
    };
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
