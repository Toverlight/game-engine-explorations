#include "scene_main.hpp"
#include "scene_title.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "world/effect.hpp"
#include "spawner.hpp"
#include "screen/ui_mouse.hpp"
#include "world/spell.hpp"
#include "screen/hud_stats.hpp"
#include "screen/hud_text.hpp"
#include "screen/hud_button.hpp"
#include "raw/timer.hpp"
#include "raw/bg_star.hpp"
#include <fstream>

void SceneMain::init()
{
    SDL_HideCursor();
    game_.playMusic("assets/bgm/OhMyGhost.ogg");
    world_size_ = game_.getScreenSize() * 3.0f;
    camera_position_ = world_size_ / 2.0f - game_.getScreenSize() / 2.0f;
    player_ = new Player();
    player_->init();
    player_->setPosition(world_size_ / 2.0f);
    addChild(player_);

    BgStar* bg_star = BgStar::addBgStarChild(this, 2000, 0.2f, 0.5f, 0.7f);

    end_timer_ = Timer::addTimerChild(this);

    spawner_ = new Spawner();
    spawner_->init();
    spawner_->setTarget(player_);
    addChild(spawner_);

    hud_button_pause_ = HUDButton::addHUDButtonChild(this, game_.getScreenSize() - glm::vec2(230.0f, 30.f), "assets/UI/A_Pause1.png", "assets/UI/A_Pause2.png", "assets/UI/A_Pause3.png");
    hud_button_restart_ = HUDButton::addHUDButtonChild(this, game_.getScreenSize() - glm::vec2(140.0f, 30.f), "assets/UI/A_Restart1.png", "assets/UI/A_Restart2.png", "assets/UI/A_Restart3.png");
    hud_button_back_ = HUDButton::addHUDButtonChild(this, game_.getScreenSize() - glm::vec2(50.0f, 30.f), "assets/UI/A_Back1.png", "assets/UI/A_Back2.png", "assets/UI/A_Back3.png");

    hud_stats_ = HUDStats::addHUDStatsChild(this, player_, glm::vec2(30.0f));
    hud_text_score_ = HUDText::addHUDTextChild(this, "Score: 0", glm::vec2(game_.getScreenSize().x - 120.0f, 30.0f), glm::vec2(200.0f, 50.0f));
    ui_mouse_ = UIMouse::addUIMouseChild(this, "assets/UI/29.png", "assets/UI/30.png", 1.0f, Anchor::CENTER);

    // auto enemy = new Enemy();
    // enemy->init();
    // enemy->setMaxSpeed(300.0f);
    // enemy->setTarget(player_);
    // enemy->setPosition(world_size_ / 2.0f + glm::vec2(200.0f));
    // Effect::addEffectChild(this, "assets/effect/184_3.png", world_size_ / 2.0f + glm::vec2(200.0f), 1.0f, enemy);


}

bool SceneMain::handleEvents(SDL_Event& event) 
{
    return Scene::handleEvents(event);
}

void SceneMain::update(float dt)
{
    checkSlowDown(dt);
    Scene::update(dt);
    updateScore();
    checkButtonPause();
    checkButtonRestart();
    checkButtonBack();
    if (player_ && !player_->isActive()) 
    {
        end_timer_->start();
        saveData("assets/score.dat");
    }
    checkEndTimer();
}

void SceneMain::render()
{
    renderBackground();
    Scene::render();
}

void SceneMain::clean()
{
    Scene::clean();
}

void SceneMain::saveData(const std::string &file_path)
{
    auto score = game_.getHighScore();
    std::ofstream file(file_path, std::ios::binary); // 以二进制方式打开文件
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&score), sizeof(score)); // 写入数据
        file.close(); // 关闭文件
    }
    
}

void SceneMain::renderBackground()
{
    auto start = -camera_position_;
    auto end = world_size_ - camera_position_;
    game_.drawGrid(start, end, 80.0f, {0.5f, 0.5f, 0.5f, 1.0f});
    game_.drawBoundary(start, end, 5.0f, {1.0f, 1.0f, 1.0f, 1.0f});
}

void SceneMain::updateScore()
{
    hud_text_score_->setText("Score: " + std::to_string(game_.getScore()));
}

void SceneMain::checkButtonPause()
{
    if (!hud_button_pause_->isTrigger()) return;
    if (is_paused_) resume();
    else pause();
}

void SceneMain::checkButtonRestart()
{
    if (!hud_button_restart_->isTrigger()) return;
    saveData("assets/score.dat");
    game_.setScore(0);
    auto scene = new SceneMain();
    game_.safeChangeScene(scene);
}

void SceneMain::checkButtonBack()
{
    if (!hud_button_back_->isTrigger()) return;
    saveData("assets/score.dat");
    game_.setScore(0);
    auto scene = new SceneTitle();
    game_.safeChangeScene(scene);
}

void SceneMain::checkEndTimer()
{
    if (!end_timer_->timeOut()) return;
    pause();
    game_.resumeMusic();
    hud_button_restart_->setRenderPosition(game_.getScreenSize() / 2.0f - glm::vec2(200.0f, 0.0f));
    hud_button_restart_->setScale(4.0f);
    hud_button_back_->setRenderPosition(game_.getScreenSize() / 2.0f + glm::vec2(200.0f, 0.0f));
    hud_button_back_->setScale(4.0f);
    hud_button_pause_->setActive(false);
    end_timer_->stop();
}

void SceneMain::checkSlowDown(float& dt)
{
    if (game_.getMouseButtonState() & SDL_BUTTON_RMASK) {
        dt *= 0.4f;
    }
}
