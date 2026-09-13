#include "Heart.hpp"

void Heart::Update(float dt)
{
    if (isValid) {
        position.y += speed * dt;

        if (position.y > GetScreenHeight() + 100) GoInvalid();
    }
}

void Heart::Draw()
{
    if (isValid) {
        DrawCircleV(position, 5, GREEN);
    }
}

void Heart::Spawn(Vector2 position)
{
    this->position = position;
    speed = GetRandomValue(100, 200);
    isValid = true;
}
