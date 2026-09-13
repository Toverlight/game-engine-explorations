#ifndef WEAPON_THUNDER_HPP
#define WEAPON_THUNDER_HPP

#include "raw/weapon.hpp"
#include "screen/hud_skill.hpp"

class WeaponThunder : public Weapon {
protected:
    HUDSkill* hud_skill_ = nullptr;

public:
    static WeaponThunder* addWeaponThunderChild(Actor* parent, float cool_down = 1.0f, float mana_cost = 0.0f);

    virtual void init() override;
    virtual void update(float dt) override;
    virtual bool handleEvents(SDL_Event& event) override;

};

#endif // WEAPON_THUNDER_HPP