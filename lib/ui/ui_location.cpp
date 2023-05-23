#include "ui_location.h"

#include "lvgl.h"

UiLocation::UiLocation(lv_obj_t * parent) {
  static lv_coord_t col_dsc[] = {128, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

  lv_obj_t * cont = lv_obj_create(parent);

  lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
  lv_obj_set_size(cont, 128, 64);
  lv_obj_center(cont);
  lv_obj_set_layout(cont, LV_LAYOUT_GRID);

  radial_label_ = lv_label_create(cont);
  lv_label_set_text(radial_label_, "10:15");

  distance_label_ = lv_label_create(cont);
  lv_label_set_text(distance_label_, "1000'");

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  // lv_style_set_border_width(&style, 1);

  static lv_style_t container_style;
  lv_style_set_pad_row(&container_style, 0);
  lv_style_set_pad_all(&container_style, 0);
  lv_obj_add_style(parent, &container_style, 0);

  lv_obj_add_style(radial_label_, &style, 0);
  lv_obj_add_style(distance_label_, &style, 0);


  lv_obj_set_grid_cell(radial_label_, LV_GRID_ALIGN_CENTER, /*col=*/0, 1,
                        LV_GRID_ALIGN_CENTER, /*row=*/1, 1);
  lv_obj_set_grid_cell(distance_label_, LV_GRID_ALIGN_CENTER, /*col=*/0, 1,
                        LV_GRID_ALIGN_CENTER, /*row=*/2, 1);

  // Parent
  lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, /*col=*/0, 1,
                        LV_GRID_ALIGN_STRETCH, /*row=*/1, 1);     
}

void UiLocation::update() {
  
}