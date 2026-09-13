#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "game.hpp"
#include <vector>
#include "defs.hpp"
class Object {
protected:
    ObjectType type_ = ObjectType::NONE;
    Game& game_ = Game::GetInstance(); // 获取游戏实例
    std::vector<Object*> object_to_add_;
    std::vector<Object*> children_; // 子节点
    bool is_active_ = true;
    bool need_remove_ = false;
public:
    Object() = default;
    virtual ~Object() = default; // 约定所有的类，不在构造函数和虚构函数里做任何事

    virtual void init() {}; // 需要初始化的事物，在init()函数里面做
    virtual bool handleEvents(SDL_Event& event); // 处理事件：事件如果被处理，则返回真，否则继续向下传递
    virtual void update(float dt);
    virtual void render();
    virtual void clean(); // 需要清理的资源，在clean()函数里面做

    void safeAddChild(Object* child) { object_to_add_.push_back(child); }
    virtual void addChild(Object* child);
    virtual void removeChild(Object* child) { children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end()); }
    void setActive(bool active) {is_active_ = active;}
    bool isActive() const { return is_active_; }

    // getters and setters
    ObjectType getType() const { return type_; }
    void setType(ObjectType type) { type_ = type; }
    bool getNeedRemove() const { return need_remove_; }
    void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }
};

#endif // OBJECT_HPP