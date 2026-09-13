#ifndef HUD_SKILL_HPP
#define HUD_SKILL_HPP

#include "../core/object_screen.hpp"

class Sprite;
class HUDSkill : public ObjectScreen {
protected:
    Sprite* icon_ = nullptr; // 技能图标
    float percentage_ = 1.0f;

public:
    static HUDSkill* addHudSkillChild(Object* parent, const std::string& file_path, glm::vec2 pos, float scale = 1.0f, Anchor anchor = Anchor::CENTER);
    virtual void render() override;

    // setters and getters
    Sprite* getIcon() const { return icon_; }
    void setIcon(Sprite* icon) { icon_ = icon; }
    float getPercentage() const { return percentage_; }
    void setPercentage(float percentage);

};

#endif // HUD_SKILL_HPP