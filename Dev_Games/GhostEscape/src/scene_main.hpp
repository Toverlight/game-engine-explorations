#ifndef SCENE_MAIN_HPP
#define SCENE_MAIN_HPP

#include "core/scene.hpp"
#include "player.hpp"

class Spawner;
class Player;
class UIMouse;
class HUDStats;
class HUDText;
class HUDButton;
class Timer;
class SceneMain : public Scene {
    Player* player_ = nullptr; // 玩家
    Spawner* spawner_ = nullptr;
    UIMouse* ui_mouse_ = nullptr; // 鼠标UI
    HUDStats* hud_stats_ = nullptr; // HUD状态UI
    HUDText* hud_text_score_ = nullptr; // HUD分数UI
    HUDButton* hud_button_pause_ = nullptr; // 暂停按钮
    HUDButton* hud_button_restart_ = nullptr; // 重新开始按钮
    HUDButton* hud_button_back_ = nullptr; // 返回按钮
    Timer* end_timer_ = nullptr; 
public:
    SceneMain() = default;
    virtual ~SceneMain() = default;

    virtual void init() override;
    virtual bool handleEvents(SDL_Event& event) override;
    virtual void update(float dt) override;
    virtual void render() override;
    virtual void clean() override;

    virtual void saveData(const std::string& file_path) override;

private:
    void renderBackground();
    void updateScore();
    void checkButtonPause();
    void checkButtonRestart();
    void checkButtonBack();
    void checkEndTimer();
    void checkSlowDown(float& dt);
};

#endif // SCENE_MAIN_HPP
