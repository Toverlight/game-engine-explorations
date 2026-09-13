#include "FallingStone.hpp"
#include <cmath>

void FallingStone::GenerateVerticesOffsets(int numVertices)
{
    radius = GetRandomValue(RADIUS_MAX * 0.4f, RADIUS_MAX * 0.8f);
    verticesOffsets.clear();
    float baseAngle = GetRandomValue(0, 359);
    for (int i = 0; i < numVertices; i++) {
        float theta = baseAngle + (i * 360.0f / numVertices);
        float rad = theta * DEG2RAD;
        float r = radius * (0.8f + GetRandomValue(0, 20) * 1.0f / RADIUS_MAX);
        verticesOffsets.push_back({r * cos(rad), r * sin(rad)});
    }
}

Vector2 FallingStone::RotateVertex(Vector2 v)
{
    float rad = rotation * DEG2RAD;
    return {
        v.x * cos(rad) - v.y * sin(rad) + position.x,
        v.x * sin(rad) + v.y * cos(rad) + position.y
    };
}

void FallingStone::Draw()
{
    int count = verticesOffsets.size();
    for (int i = 0; i < count; i++) {
        int next = (i + 1) % count;
        Vector2 v1 = RotateVertex(verticesOffsets[i]);
        Vector2 v2 = RotateVertex(verticesOffsets[next]);
        DrawLineV(v1, v2, WHITE);
    }
}

void FallingStone::Update(float dt)
{
    position.y += fallingSpeed * dt;
    rotation += omega * dt;
    if (abs(rotation) > 180) {
        if (rotation > 0) rotation -= 360;
        else if (rotation < 0) rotation += 360;
    }
    
    if (position.y - radius > GetScreenHeight()) {
        Regenerate();
    }
}

bool FallingStone::PointInside(Vector2 point)
{
    int intersections = 0;
    int count = verticesOffsets.size();
    for (int i = 0; i < count; i++) {
        int next = (i + 1) % count;
        Vector2 v1 = RotateVertex(verticesOffsets[i]);
        Vector2 v2 = RotateVertex(verticesOffsets[next]);
        Vector2 collisionPoint;
        if (CheckCollisionLines(v1, v2, position, point, &collisionPoint)) intersections++;
    }
    return intersections == 0;
}

bool FallingStone::CircleInside(Vector2 center, float radius)
{
    int count = verticesOffsets.size();
    for (int i = 0; i < count; i++) {
        int next = (i + 1) % count;
        Vector2 v1 = RotateVertex(verticesOffsets[i]);
        Vector2 v2 = RotateVertex(verticesOffsets[next]);
        if (CheckCollisionPointLine(center, v1, v2, radius)) return true;
    }
    return false;
}

void FallingStone::Regenerate()
{
    position.y = GetRandomValue(-300, -100);
    position.x = GetRandomValue(RADIUS_MAX, GetScreenWidth() - RADIUS_MAX);
    GenerateVerticesOffsets(GetRandomValue(4, 6));
}
