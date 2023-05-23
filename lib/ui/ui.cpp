#include "ui.h"

#include "lvgl.h"
#include "ui_location.h"
#include "ui_waypoint.h"

Ui::Ui() {
  static lv_style_t title_style;
  lv_style_init(&title_style);
  lv_style_set_text_font(&title_style, &lv_font_unscii_8);

  static lv_coord_t col_dsc[] = {128, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {14, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

  const char* kTitles[] = {"LOCATION", "WAYPOINT"};
  for (int i = 0; i < 2; ++i) {
    screens_[i] = lv_obj_create(nullptr);

    screen_labels_[i] = lv_label_create(screens_[i]);
    lv_label_set_text(screen_labels_[i], kTitles[i]);
    lv_obj_add_style(screen_labels_[i], &title_style, 0);

    lv_obj_set_style_grid_column_dsc_array(screens_[i], col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(screens_[i], row_dsc, 0);
    lv_obj_set_size(screens_[i], 128, 64);
    lv_obj_center(screens_[i]);
    lv_obj_set_layout(screens_[i], LV_LAYOUT_GRID);

    lv_obj_set_grid_cell(screen_labels_[i], LV_GRID_ALIGN_CENTER, /*col=*/0, 1,
                         LV_GRID_ALIGN_STRETCH, /*row=*/0, 1);
  }

  ui_location_ = new UiLocation(screens_[0]);
  // ui_waypoint_ = new UiWaypoint(screens_[1]);

  lv_scr_load(screens_[0]);
}

void Ui::SetMode(Mode mode) {
  // switch (mode) {
  //   case Mode::kLocation:
  //     lv_scr_load(screens_[0]);
  //     break;
  //   case Mode::kWaypoint:
  //     lv_scr_load(screens_[1]);
  //     break;
  //   default:
  //     break;
  // }
}
