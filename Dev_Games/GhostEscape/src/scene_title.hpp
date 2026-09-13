#ifndef SCENE_TITLE_HPP
#define SCENE_TITLE_HPP

#include "core/scene.hpp"

class HUDButton;
class HUDText;
class UIMouse;
class SceneTitle : public Scene {
protected:
    SDL_FColor boundary_color_ = {0.5f, 0.5f, 0.5f, 1.0f};
    float color_timer_ = 0;
    HUDButton* start_button_ = nullptr;
    HUDButton* credits_button_ = nullptr;
    HUDButton* quit_button_ = nullptr;
    HUDText* credits_text_ = nullptr;
    UIMouse* ui_mouse_ = nullptr;

public:
    virtual void init() override;
    virtual bool handleEvents(SDL_Event& event) override;
    virtual void update(float dt) override;
    virtual void render() override;
    virtual void clean() override;

    virtual void loadData(const std::string& file_path) override;

private:
    void renderBackground();
    void updateColor();
    void checkButtonQuit();
    void checkButtonStart();
    void checkButtonCredits();
    
};

#endif // SCENE_TITLE_HPP