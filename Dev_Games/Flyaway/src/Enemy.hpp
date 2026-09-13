#pragma once
#include <raylib.h>

class Enemy {
    public:
        void Update(float dt, int &score);
        void Draw();
        void Regenerate();
        bool PointInside(Vector2 point);
        void Light()
        {
            isLighting = true;
            color = ORANGE;
            hp--;
        }
        void LightIfCondition(float dt, float lightTime);
    private:
        Vector2 position;
        Vector2 velocity;
        Vector2 verticesOffsets[3];
        Color color{GRAY};
        int hpMax{GetRandomValue(3,6)};
        int hp{hpMax};
        bool isLighting{false};
};