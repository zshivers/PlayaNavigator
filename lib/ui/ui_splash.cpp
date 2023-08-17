#include "ui_splash.h"

#include <array>

LV_IMG_DECLARE(NaviGator00);
LV_IMG_DECLARE(NaviGator01);
LV_IMG_DECLARE(NaviGator02);
LV_IMG_DECLARE(NaviGator03);
LV_IMG_DECLARE(NaviGator04);
LV_IMG_DECLARE(NaviGator05);
LV_IMG_DECLARE(NaviGator06);
LV_IMG_DECLARE(NaviGator07);
LV_IMG_DECLARE(NaviGator08);
LV_IMG_DECLARE(NaviGator09);
LV_IMG_DECLARE(NaviGator10);
LV_IMG_DECLARE(NaviGator11);
LV_IMG_DECLARE(NaviGator12);
LV_IMG_DECLARE(NaviGator13);
LV_IMG_DECLARE(NaviGator14);
LV_IMG_DECLARE(NaviGator15);
LV_IMG_DECLARE(NaviGator16);
LV_IMG_DECLARE(NaviGator17);
LV_IMG_DECLARE(NaviGator18);
LV_IMG_DECLARE(NaviGator19);

namespace {
constexpr std::array<const lv_img_dsc_t *, 20> kNaviGatorAnimationImages = {
    &NaviGator00, &NaviGator01, &NaviGator02, &NaviGator03, &NaviGator04,
    &NaviGator05, &NaviGator06, &NaviGator07, &NaviGator08, &NaviGator09,
    &NaviGator10, &NaviGator11, &NaviGator12, &NaviGator13, &NaviGator14,
    &NaviGator15, &NaviGator16, &NaviGator17, &NaviGator18, &NaviGator19,
};

constexpr int32_t kSplashAnimationDuration_ms = 2'000;
}  // namespace

UiSplash::UiSplash() {
  screen_ = lv_obj_create(nullptr);
  animimg_ = lv_animimg_create(screen_);
  lv_obj_center(animimg_);
  lv_animimg_set_src(animimg_, (const void **)kNaviGatorAnimationImages.data(), kNaviGatorAnimationImages.size());
  lv_animimg_set_duration(animimg_, kSplashAnimationDuration_ms);
  lv_animimg_set_repeat_count(animimg_, 1);
}

void UiSplash::Start() { lv_animimg_start(animimg_); }

bool UiSplash::Complete() {
  // This is a hack. There are two issues:
  // 1.  Cannot bind a C-style callback into a C++ member variable, so cannot
  // access the various animation callbacks available.
  // 2. There is a bug in the library where the first image is shown again at
  // the end of the animation. The splash screen is finished when the image
  // being shown is the last one.
  lv_animimg_t *ai = (lv_animimg_t *)animimg_;
  return ai->img.src == kNaviGatorAnimationImages.back();
}