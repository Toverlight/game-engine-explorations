#pragma once
#include <raylib.h>

class Heart {
    public:
        
        void Update(float dt);
        void Draw();
        bool IsValid() {return isValid;}
        void GoInvalid() {isValid = false;}
        void Spawn(Vector2 position);
        Vector2 GetPosition() {return position;};
    private:
        Vector2 position;
        float speed{(float)GetRandomValue(100, 200)};
        Color color{RED};
        bool isValid{false};
};