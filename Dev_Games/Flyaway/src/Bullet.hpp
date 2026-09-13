#pragma once
#include <raylib.h>

class FallingStone;
class Enemy;

class Bullet {
    public:
        void Update(float dt);
        void Draw();
        void DetectAndDestroy(FallingStone& fs, int& score);
        void DetectAndHarm(Enemy& enemy);
        bool IsValid();
        void GoInvalid();
        void Spawn(Vector2 position);
        
    private:
        Vector2 position;
        float speed{-200.0f};
        Color color{WHITE};
        float radius{5};
        bool isValid{false};

};