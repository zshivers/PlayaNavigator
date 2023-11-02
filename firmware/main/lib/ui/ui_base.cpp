#include "ui_base.h"

#include <string>

UiBase::UiBase() {
  screen_ = lv_obj_create(nullptr);

  window_ = lv_win_create(screen_, 10);
  title_ = lv_win_add_title(window_, "");
  content_ = lv_win_get_content(window_);

  // White title text, centered.
  lv_style_init(&title_style_);
  lv_style_set_text_font(&title_style_, &lv_font_unscii_8);
  lv_style_set_text_color(&title_style_, lv_color_white());
  lv_style_set_text_align(&title_style_, LV_TEXT_ALIGN_CENTER);
  lv_obj_add_style(title_, &title_style_, 0);

  // Black background for header.
  lv_style_init(&header_style_);
  lv_style_set_bg_color(&header_style_, lv_color_black());
  lv_obj_add_style(lv_win_get_header(window_), &header_style_, 0);
}

void UiBase::SetTitle(std::string title) {
  lv_label_set_text(title_, title.c_str());
}