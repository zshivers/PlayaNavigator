#include "ui_waypoint.h"

#include "lvgl.h"

UiWaypoint::UiWaypoint(lv_obj_t* parent) {
  static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

  lv_obj_t * cont = lv_obj_create(parent);

  lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
  lv_obj_set_size(cont, 128, 64);
  lv_obj_center(cont);
  lv_obj_set_layout(cont, LV_LAYOUT_GRID);

  time_label_ = lv_label_create(cont);
  lv_label_set_text(time_label_, "1500 ft");

  static lv_style_t title_style;
  lv_style_init(&title_style);
  lv_style_set_text_font(&title_style, &lv_font_unscii_8);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  // lv_style_set_border_width(&style, 1);

  static lv_style_t container_style;
  lv_style_set_pad_row(&container_style, 0);
  lv_style_set_pad_all(&container_style, 0);
  lv_obj_add_style(cont, &container_style, 0);

  direction_meter_ = lv_meter_create(cont);
  lv_meter_scale_t * scale = lv_meter_add_scale(direction_meter_);
  lv_meter_set_scale_ticks(direction_meter_, scale, 13, 2, 10, lv_color_black());
  lv_meter_set_scale_major_ticks(direction_meter_, scale, 1, 1, 5, lv_color_black(), 0);
  lv_meter_set_scale_range(direction_meter_, scale, 0, 360, 360, 0);
  lv_obj_set_size(direction_meter_, 45, 45);

  lv_obj_add_style(time_label_, &style, 0);

  lv_obj_set_grid_cell(time_label_, LV_GRID_ALIGN_STRETCH, /*col=*/0, 1,
                        LV_GRID_ALIGN_STRETCH, /*row=*/0, 1);
  lv_obj_set_grid_cell(direction_meter_, LV_GRID_ALIGN_CENTER, /*col=*/1, 1,
                        LV_GRID_ALIGN_CENTER, /*row=*/0, 1);

  // Parent
  lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, /*col=*/0, 1,
                        LV_GRID_ALIGN_STRETCH, /*row=*/1, 1);     
}

void UiWaypoint::update() {}