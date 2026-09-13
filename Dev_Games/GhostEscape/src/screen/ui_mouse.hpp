#ifndef UI_MOUSE_HPP
#define UI_MOUSE_HPP

#include "../core/object_screen.hpp"
#include "../affiliate/sprite.hpp"

class UIMouse : public ObjectScreen {
protected:
    Sprite* sprite1_ = nullptr;
    Sprite* sprite2_ = nullptr;
    float timer_ = 0;

public:
    static UIMouse* addUIMouseChild(Object* parent, const std::string& file_path1, const std::string& file_path2, float scale = 1.0f, Anchor anchor = Anchor::CENTER);
    virtual void update(float dt) override;

    // setters and getters
    Sprite* getSprite1() const { return sprite1_; }
    void setSprite1(Sprite* sprite) { sprite1_ = sprite; }
    Sprite* getSprite2() const { return sprite2_; }
    void setSprite2(Sprite* sprite) { sprite2_ = sprite; }

};

#endif // UI_MOUSE_HPP