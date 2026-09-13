#include "weapon_thunder.hpp"
#include "world/spell.hpp"
#include "core/scene.hpp"
#include "core/actor.hpp"

WeaponThunder *WeaponThunder::addWeaponThunderChild(Actor *parent, float cool_down, float mana_cost)
{
    auto weapon = new WeaponThunder();
    weapon->init();
    weapon->setParent(parent);
    weapon->setCoolDown(cool_down);
    weapon->setManaCost(mana_cost);
    if (parent) parent->addChild(weapon);
    return weapon;
}

void WeaponThunder::init()
{
    Weapon::init();
    auto scene = game_.getCurrentScene();
    auto pos = glm::vec2(game_.getScreenSize().x - 300, 30);
    hud_skill_ = HUDSkill::addHudSkillChild(scene, "assets/UI/Electric-Icon.png", pos, 0.14f, Anchor::CENTER);
}

void WeaponThunder::update(float dt)
{
    Weapon::update(dt);
    if (hud_skill_) {
        hud_skill_->setPercentage(cool_down_timer_ / cool_down_);
    }
    
}

bool WeaponThunder::handleEvents(SDL_Event &event)
{
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (canAttack()) {
                game_.playSound("assets/sound/big-thunder.mp3");
                auto pos = game_.getMousePosition() + game_.getCurrentScene()->getCameraPosition();
                auto spell = Spell::addSpellChild(nullptr, "assets/effect/Thunderstrike w blur.png", pos, 40.0f, 3.0f, Anchor::CENTER);
                attack(pos, spell);
                return true;
            }
        }
    }
    return false;
}
