#ifndef OBJECT_SCREEN_HPP
#define OBJECT_SCREEN_HPP

#include "object.hpp"

class ObjectScreen : public Object
{
protected:
    glm::vec2 render_position_ = glm::vec2(0, 0); // 渲染（屏幕）位置

public:
    virtual void init() override {type_ = ObjectType::OBJECT_SCREEN;}

    // getters and setters
    glm::vec2 getRenderPosition() const { return render_position_; }
    virtual void setRenderPosition(const glm::vec2& position) { render_position_ = position; }
    virtual glm::vec2 getPosition() const { return glm::vec2(0); }

};

#endif // OBJECT_SCREEN_HPP