#pragma once

class Vector2F
{
public:
    float x, y;

    Vector2F(float x, float y) : x(x), y(y) {}

    static Vector2F zero()
    {
        return Vector2F(0.0, 0.0);
    }

    Vector2F &operator+=(const Vector2F &a)
    {
        this->x += a.x;
        this->y += a.y;
        return *this;
    }

    Vector2F &operator-=(/* Vector2F this, */ const Vector2F &a)
    {
        this->x -= a.x;
        this->y -= a.y;
        return *this;
    }

    Vector2F &operator*=(const float &b)
    {
        this->x *= b;
        this->y *= b;
        return *this;
    }

    Vector2F &operator/=(const float &b)
    {
        this->x /= b;
        this->y /= b;
        return *this;
    }

    bool operator==(Vector2F const &b) const { return this->x == b.x && this->y == b.y; }
    bool operator!=(Vector2F const &b) const { return !(*this == b); }
};

Vector2F operator+(Vector2F a, const Vector2F &b)
{
    return a += b;
}

Vector2F operator-(Vector2F a, const Vector2F &b)
{
    return a -= b;
}

Vector2F operator*(Vector2F a, const float &b)
{
    return a *= b;
}

Vector2F operator/(Vector2F a, const float &b)
{
    return a /= b;
}
