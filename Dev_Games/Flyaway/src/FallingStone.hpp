#pragma once
#include <raylib.h>
#include <vector>
#define RADIUS_MAX 100
using namespace std;

class FallingStone {
    public:
        FallingStone(Vector2 position, int numVertices): position(position)
        {
            fallingSpeed = GetRandomValue(1000, 2000) / 10.0f;
            rotation = GetRandomValue(0, 359);
            omega = GetRandomValue(-3000, 3000) / 30.0f;
            GenerateVerticesOffsets(numVertices);
        }

        void GenerateVerticesOffsets(int numVertices);
        Vector2 RotateVertex(Vector2 v);
        void Draw();
        void Update(float dt);
        bool PointInside(Vector2 point);
        bool CircleInside(Vector2 center, float radius);
        void Regenerate();

    private:
        Vector2 position;
        float fallingSpeed;
        float rotation;
        float omega;
        vector<Vector2> verticesOffsets;
        float radius;
};