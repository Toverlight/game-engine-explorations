#include "bg_star.hpp"
#include "../core/scene.hpp"

BgStar *BgStar::addBgStarChild(Object *parent, int num, float scale_far, float scale_mid, float scale_near)
{
    BgStar* bg_star = new BgStar();
    bg_star->init();
    bg_star->num_ = num;
    bg_star->scale_far_ = scale_far;
    bg_star->scale_mid_ = scale_mid;
    bg_star->scale_near_ = scale_near;
    bg_star->star_far_.reserve(num);
    bg_star->star_mid_.reserve(num);
    bg_star->star_near_.reserve(num);
    auto extra = Game::GetInstance().getCurrentScene()->getWorldSize() - Game::GetInstance().getScreenSize();
    for (int i = 0; i < num; ++i) {
        bg_star->star_far_.push_back(Game::GetInstance().getRandomVec2(glm::vec2(0), Game::GetInstance().getScreenSize() + extra * scale_far));
        bg_star->star_mid_.push_back(Game::GetInstance().getRandomVec2(glm::vec2(0), Game::GetInstance().getScreenSize() + extra * scale_mid));
        bg_star->star_near_.push_back(Game::GetInstance().getRandomVec2(glm::vec2(0), Game::GetInstance().getScreenSize() + extra * scale_near));
    }
    if (parent) parent->addChild(bg_star);
    return bg_star;
}

void BgStar::update(float dt)
{
    // 更新颜色
    timer_ += dt;
    color_far_ = {0.5f + 0.5f * sinf(timer_ * 0.9f), 0.5f + 0.5f * sinf(timer_ * 0.8f), 0.5f + 0.5f * sinf(timer_ * 0.7f), 1.0f};
    color_mid_ = {0.5f + 0.5f * sinf(timer_ * 0.8f), 0.5f + 0.5f * sinf(timer_ * 0.7f), 0.5f + 0.5f * sinf(timer_ * 0.6f), 1.0f};
    color_near_ = {0.5f + 0.5f * sinf(timer_ * 0.7f), 0.5f + 0.5f * sinf(timer_ * 0.6f), 0.5f + 0.5f * sinf(timer_ * 0.5f), 1.0f};
}

void BgStar::render()
{
    game_.drawPoints(star_far_, - game_.getCurrentScene()->getCameraPosition() * scale_far_, color_far_);
    game_.drawPoints(star_mid_, - game_.getCurrentScene()->getCameraPosition() * scale_mid_, color_mid_);
    game_.drawPoints(star_near_, - game_.getCurrentScene()->getCameraPosition() * scale_near_, color_near_);
}
