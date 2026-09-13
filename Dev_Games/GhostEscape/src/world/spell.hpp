#ifndef SPELL_HPP
#define SPELL_HPP

#include "../core/object_world.hpp"
#include "../affiliate/sprite_anim.hpp"

class Spell : public ObjectWorld {

protected:
    SpriteAnim* sprite_ = nullptr; // 精灵动画
    float damage_ = 60.0f; // 伤害

public:

    static Spell* addSpellChild(Object* parent, const std::string& file_path, glm::vec2 pos, float damage, float scale = 1.0f, Anchor anchor = Anchor::CENTER); 
    virtual void update(float dt) override; // 更新

    void setDamage(float damage) { damage_ = damage; } // 设置伤害
    float getDamage() const { return damage_; } // 获取伤害
    void setSpriteAnim(SpriteAnim* sprite) { sprite_ = sprite; } // 设置精灵动画
    SpriteAnim* getSpriteAnim() const { return sprite_; } // 获取精灵动画


private:
    void attack(); // 攻击
};



#endif // SPELL_HPP