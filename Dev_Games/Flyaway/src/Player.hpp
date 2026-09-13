#pragma once
#include <raylib.h>
#include <vector>
using namespace std;

class FallingStone;
class Bullet;

enum {
    RIGHT,
    UP,
    LEFT
};
class Player {
    public:
        Player(Vector2 position): position(position), statusColor(WHITE), isFlashing(false) {}
        void Draw();
        void Move();
        void Update();
        bool IfHitFallingStone(FallingStone& fs);
        void FlashIfCondition(float dt, float flashTime);
        void GoRed();
        void GoWhite();
        bool IsFlashing();
        void Flash();
        void HandleShooting(vector<Bullet>& bullets);
        bool PointInTriangle(Vector2 point);
    private:
        Vector2 position;
        Vector2 bodyVerticesOffsets[3]{{70, 0}, {0, -50}, {-70, 0}};
        Vector2 velocity{0, 0};
        Color statusColor;
        bool isFlashing;
};