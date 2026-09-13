#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "../core/object_world.hpp"
#include "../affiliate/sprite_anim.hpp"

class Effect : public ObjectWorld {
    SpriteAnim* sprite_ = nullptr;
    ObjectWorld* next_object_ = nullptr; // 播放完毕后添加到场景中。
public:
    static Effect* addEffectChild(Object* parent, const std::string& file_path, glm::vec2 pos, float scale = 1.0f, ObjectWorld* next_object = nullptr);
    virtual void update(float dt) override;
    virtual void clean() override;

    // setters and getters
    SpriteAnim* getSprite() const { return sprite_; }
    void setSprite(SpriteAnim* sprite) { sprite_ = sprite; }
    ObjectWorld* getNextObject() const { return next_object_; }
    void setNextObject(ObjectWorld* object) { next_object_ = object; }

private:
    void checkFinish();
};


#endif // EFFECT_HPP