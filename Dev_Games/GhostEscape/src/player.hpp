#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "core/actor.hpp"
#include "affiliate/sprite_anim.hpp"
#include "world/effect.hpp"
#include "weapon_thunder.hpp"

class Timer;
class Player : public Actor {
    WeaponThunder* weapon_thunder_ = nullptr;
    SpriteAnim* sprite_idle_ = nullptr;
    SpriteAnim* sprite_move_ = nullptr;
    bool is_moving_ = false;
    Effect* effect_ = nullptr;
    Timer* flash_timer_ = nullptr;
public:
    Player() = default;
    ~Player() = default;

    virtual void init() override;
    virtual bool handleEvents(SDL_Event& event) override;
    virtual void update(float dt) override;
    virtual void render() override;
    virtual void clean() override;

    virtual void takeDamage(float damage) override;

    void keyboardControl();
    void syncCamera();
    void checkState();
    void changeState(bool is_moving);
    void checkIsDead();
};

#endif // PLAYER_HPP