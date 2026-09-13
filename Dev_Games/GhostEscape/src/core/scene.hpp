#ifndef SCENE_HPP
#define SCENE_HPP

#include "object_world.hpp"
#include <glm/glm.hpp>
#include <vector>

class Scene : public Object {
protected:
    glm::vec2 world_size_ = glm::vec2(0); // 世界尺寸
    glm::vec2 camera_position_ = glm::vec2(0); // 相机位置
    std::vector<ObjectWorld*> children_worlds_;
    std::vector<ObjectScreen*> children_screens_;
    bool is_paused_ = false; // 是否暂停

  public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void init() override {}
    virtual bool handleEvents(SDL_Event& event) override;
    virtual void update(float dt) override;
    virtual void render() override;
    virtual void clean() override;

    virtual void addChild(Object* child) override;
    virtual void removeChild(Object* child) override; // 只是把容器中的元素从容器中移除，并没有delete该对象

    virtual void saveData(const std::string& file_path) {};
    virtual void loadData(const std::string& file_path) {};

    glm::vec2 worldToScreen(const glm::vec2 &world_position) const { return world_position - camera_position_; }
    glm::vec2 screenToWorld(const glm::vec2 &screen_position) const { return screen_position + camera_position_; }

    void pause();
    void resume();

    // getters and setters
    glm::vec2 getCameraPosition() const { return camera_position_; }
    void setCameraPosition(const glm::vec2& camera_position);
    glm::vec2 getWorldSize() const { return world_size_; }
    void setWorldSize(const glm::vec2& size) { world_size_ = size; }
    std::vector<ObjectWorld *>& getChildrenWorlds() { return children_worlds_; }
    std::vector<ObjectScreen *>& getChildrenScreens() { return children_screens_; }
};

#endif // SCENE_HPP