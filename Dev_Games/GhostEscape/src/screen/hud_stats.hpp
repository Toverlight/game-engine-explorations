#ifndef HUD_STATS_HPP
#define HUD_STATS_HPP

#include "../core/object_screen.hpp"

class Sprite;
class Actor;
class HUDStats : public ObjectScreen {
protected:
    Actor* target_ = nullptr;
    Sprite* health_bar_ = nullptr;
    Sprite* health_bar_bg_ = nullptr;
    Sprite* health_icon_ = nullptr;
    Sprite* mana_bar_ = nullptr;
    Sprite* mana_bar_bg_ = nullptr;
    Sprite* mana_icon_ = nullptr;

    float health_percentage_ = 1.0f;
    float mana_percentage_ = 1.0f;

public:
    static HUDStats* addHUDStatsChild(Object* parent, Actor* target, glm::vec2 render_position);
    virtual void init() override;
    virtual void update(float dt) override;

    // setters and getters
    Sprite* getHealthBar() const { return health_bar_; }
    Sprite* getHealthBarBg() const { return health_bar_bg_; }
    Sprite* getHealthIcon() const { return health_icon_; }
    Actor* getTarget() const { return target_; }
    void setTarget(Actor* target) { target_ = target; }
    Sprite* getManaBar() const { return mana_bar_; }
    Sprite* getManaBarBg() const { return mana_bar_bg_; }
    Sprite* getManaIcon() const { return mana_icon_; }
    void setHealthPercentage(float percentage) { health_percentage_ = percentage; }
    void setManaPercentage(float percentage) { mana_percentage_ = percentage; }
    float getHealthPercentage() const { return health_percentage_; }
    float getManaPercentage() const { return mana_percentage_; }

private:
    void update_health_bar();
    void update_mana_bar();

};

#endif // HUD_STATS_HPP