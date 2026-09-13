#ifndef HUD_BUTTON_HPP
#define HUD_BUTTON_HPP

#include "../core/object_screen.hpp"
#include "../affiliate/sprite.hpp"

class HUDButton : public ObjectScreen
{
protected:
    Sprite* sprite_normal_ = nullptr;
    Sprite* sprite_hover_ = nullptr;
    Sprite* sprite_press_ = nullptr;
    Sprite* current_sprite_ = nullptr;
    bool is_hover_ = false;
    bool is_press_ = false;
    bool is_trigger_ = false;

public:
    static HUDButton* addHUDButtonChild(Object* parent, glm::vec2 render_pos, const std::string& file_path_normal, const std::string& file_path_hover, const std::string& file_path_press, float scale = 1.0f, Anchor anchor = Anchor::CENTER);

    virtual bool handleEvents(SDL_Event &event) override;
    virtual void update(float) override;
    void checkHover();
    void checkState();

    // setters and getters
    bool isPress() const { return is_press_; }
    bool isHover() const { return is_hover_; }
    bool isTrigger(); // 只要出发一次，就会重置is_trigger_状态
    void setIsTrigger(bool trigger) { is_trigger_ = trigger; }
    void setIsHover(bool hover) { is_hover_ = hover; }
    void setIsPress(bool press) { is_press_ = press; }
    void setScale(float scale);
    Sprite* getSpriteNormal() const { return sprite_normal_; }
    Sprite* getSpriteHover() const { return sprite_hover_; } 
    Sprite* getSpritePress() const { return sprite_press_; }
};

#endif // HUD_BUTTON_HPP