#include "ui_base.h"

#include <string>

UiBase::UiBase() {
  screen_ = lv_obj_create(nullptr);

  // White title text, centered.
  static lv_style_t title_style;
  lv_style_init(&title_style);
  lv_style_set_text_font(&title_style, &lv_font_unscii_8);
  lv_style_set_text_color(&title_style, lv_color_white());
  lv_style_set_text_align(&title_style, LV_TEXT_ALIGN_CENTER);

  // Black background for header.
  static lv_style_t header_style;
  lv_style_init(&header_style);
  lv_style_set_bg_color(&header_style, lv_color_black());

  window_ = lv_win_create(screen_, 10);
  title_ = lv_win_add_title(window_, "");
  content_ = lv_win_get_content(window_);

  lv_obj_add_style(title_, &title_style, 0);
  lv_obj_add_style(lv_win_get_header(window_), &header_style, 0);
}

void UiBase::SetTitle(std::string title) {
  lv_label_set_text(title_, title.c_str());
}