#include "affiliate_bar.hpp"

AffiliateBar *AffiliateBar::addAffiliateBarChild(ObjectScreen *parent, glm::vec2 size, Anchor anchor)
{
    auto affiliate_bar = new AffiliateBar();
    affiliate_bar->init();
    affiliate_bar->setAnchor(anchor);
    affiliate_bar->setSize(size);
    if (parent) {
        affiliate_bar->setParent(parent);
        parent->addChild(affiliate_bar);
    }
    return affiliate_bar;
}

void AffiliateBar::render()
{
    auto pos = parent_->getRenderPosition() + offset_;
    if (percentage_ > 0.7f) {
        game_.renderHBar(pos, size_, percentage_, color_high_);
    } else if (percentage_> 0.3f) {
        game_.renderHBar(pos, size_, percentage_, color_mid_);
    } else {
        game_.renderHBar(pos, size_, percentage_, color_low_);
    }
}
