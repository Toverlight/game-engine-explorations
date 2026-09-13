#include "Bullet.hpp"
#include "FallingStone.hpp"
#include "Enemy.hpp"

void Bullet::Update(float dt)
{
    if (isValid) {
        position.y += speed * dt;

        if (position.y < -GetScreenHeight()) {
            GoInvalid();
        }
    }
}

void Bullet::Draw()
{
    if (isValid) {
        DrawCircleV(position, radius, color);
    }
}

void Bullet::DetectAndDestroy(FallingStone &fs, int &score)
{
    if (!isValid) return;
    if (fs.CircleInside(position, radius)) {
        fs.Regenerate();
        score++;
        GoInvalid();
    }
}

void Bullet::DetectAndHarm(Enemy &enemy)
{
    if (!isValid) return;
    if (enemy.PointInside(position)) {
        enemy.Light();
        GoInvalid();
    }
}

bool Bullet::IsValid()
{
    return isValid;
}

void Bullet::GoInvalid()
{
    isValid = false;
}

void Bullet::Spawn(Vector2 position)
{
    this->position = position;
    isValid = true;
}
