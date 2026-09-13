#include "Enemy.hpp"
#include <math.h>

void Enemy::Update(float dt, int& score)
{
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (hp <= 0) {
        Regenerate();
        score += 3;
    } else if (position.x > GetScreenWidth() + 100 || position.x < 100 
    || position.y > GetScreenHeight() + 200) {
        Regenerate();
    }
}

void Enemy::Draw()
{
    Vector2 vertices[3];
    for (int i = 0; i < 3; i++) {
        vertices[i].x = position.x + verticesOffsets[i].x;
        vertices[i].y = position.y + verticesOffsets[i].y;
    }
    DrawTriangle(vertices[0], vertices[1], vertices[2], color);
}

void Enemy::Regenerate()
{
    position.x = (float)GetRandomValue(40, GetScreenWidth() - 40);
    position.y = (float)GetRandomValue(-200, -50);
    float randSqrtVx = GetRandomValue(-60, 60) / 10.0f;
    velocity.x = randSqrtVx * randSqrtVx;
    velocity.y = (float)GetRandomValue(60, 180);
    float wingWidth = 50.0f + GetRandomValue(-20, 50);
    float centerLength = 30.0f + GetRandomValue(10, 50);
    verticesOffsets[0] = {-wingWidth, 0};
    verticesOffsets[1] = {0, centerLength};
    verticesOffsets[2] = {wingWidth, 0};
    color = GRAY;
    hpMax = GetRandomValue(3, 6);
    hp = hpMax;
    isLighting = false;
}

bool Enemy::PointInside(Vector2 center)
{
    Vector2 vertices[3];
    for (int i = 0; i < 3; i++)
    {
        vertices[i].x = position.x + verticesOffsets[i].x;
        vertices[i].y = position.y + verticesOffsets[i].y;
    }
    return CheckCollisionPointTriangle(center, vertices[0], vertices[1], vertices[2]);
}

void Enemy::LightIfCondition(float dt, float lightTime)
{
    static float passTime = 0;
    if (!isLighting) return;
    passTime += dt;
    if (passTime > lightTime) {
        isLighting = false;
        passTime = 0;
        color = GRAY;
    }
}
