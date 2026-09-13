#ifndef BG_STAR_HPP
#define BG_STAR_HPP

#include "../core/object.hpp"

class BgStar : public Object
{
protected:
    std::vector<glm::vec2> star_far_;
    std::vector<glm::vec2> star_mid_;
    std::vector<glm::vec2> star_near_;
    float scale_far_ = 0.2f;
    float scale_mid_ = 0.5f;
    float scale_near_ = 0.7f;
    SDL_FColor color_far_ = {0.5f, 0.5f, 0.5f, 1.0f};
    SDL_FColor color_mid_ = {0.7f, 0.7f, 0.7f, 1.0f};
    SDL_FColor color_near_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float timer_ = 0.0f;
    int num_ = 2000; // 每一层的星星数量

public:
    static BgStar* addBgStarChild(Object* parent, int num, float scale_far, float scale_mid, float scale_near);

    virtual void update(float dt) override;
    virtual void render() override;
    
};

#endif // BG_STAR_HPP