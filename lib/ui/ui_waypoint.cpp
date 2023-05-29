#include "ui_waypoint.h"

#include <cmath>
#include <limits>

#include "coordinates.h"
#include "lvgl.h"
#include "playa_config.h"
#include "waypoint_storage.h"

namespace {
constexpr uint8_t kMaxTotalWaypoints = 5;
constexpr int CANVAS_WIDTH = 40;
constexpr int CANVAS_HEIGHT = 40;

void rotate_points(lv_point_t point_in[], int point_count,
                   lv_point_t rotation_point, float angle_deg) {
  const float& x = rotation_point.x;
  const float& y = rotation_point.y;

  const float angle_rad = degToRad(angle_deg);
  const float r00 = cos(angle_rad);
  const float r01 = -sin(angle_rad);
  const float r10 = sin(angle_rad);
  const float r11 = cos(angle_rad);

  for (int i = 0; i < point_count; ++i) {
    float xin = point_in[i].x;
    float yin = point_in[i].y;
    point_in[i].x = (lv_coord_t)(r00 * xin + r01 * yin + x - r00 * x - r01 * y);
    point_in[i].y = (lv_coord_t)(r10 * xin + r11 * yin + y - r10 * x - r11 * y);
  }
}

void translate_points(lv_point_t points[], int count, lv_point_t translation) {
  for (int i = 0; i < count; ++i) {
    points[i].x += translation.x;
    points[i].y += translation.y;
  }
}

LatLon FindNearestBathroom(LatLon current_location) {
  double closest_distance = std::numeric_limits<double>::infinity();
  LatLon closest_bathroom;
  for (const LatLon bathroom : kBathroomLocations) {
    const double distance = distanceBetween(current_location, bathroom);
    if (distance < closest_distance) {
      closest_distance = distance;
      closest_bathroom = bathroom;
    }
  }
  return closest_bathroom;
}
}  // namespace

// Angle convention:
//   0 = Up
//  90 = Right
// 180 = Down
// 270 = Left
void UiWaypoint::draw_arrow(float angle_deg) {
  lv_canvas_fill_bg(canvas_, lv_color_white(), LV_OPA_COVER);

  static lv_draw_rect_dsc_t rect_dsc;
  lv_draw_rect_dsc_init(&rect_dsc);
  rect_dsc.bg_color = lv_color_black();

  lv_point_t points[] = {
      {14, 0},
      {0, 22},
      {28, 22},
  };
  translate_points(points, 3, {6, 0});
  rotate_points(points, 3, {CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2}, angle_deg);
  lv_canvas_draw_polygon(canvas_, points, 3, &rect_dsc);

  lv_point_t points2[] = {
      {8, 20},
      {8, 40},
      {20, 40},
      {20, 20},
  };
  translate_points(points2, 4, {6, 0});
  rotate_points(points2, 4, {CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2}, angle_deg);
  lv_canvas_draw_polygon(canvas_, points2, 4, &rect_dsc);
}

void UiWaypoint::draw_circle() {
  static lv_draw_arc_dsc_t arc_dsc;
  lv_draw_arc_dsc_init(&arc_dsc);
  arc_dsc.color = lv_color_black();
  arc_dsc.width = 15;

  lv_canvas_fill_bg(canvas_, lv_color_white(), LV_OPA_COVER);
  // The only way to draw a circle on a canvas is to draw an arc with a thick
  // border.
  lv_canvas_draw_arc(canvas_, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, 15, 0, 360,
                     &arc_dsc);
}

UiWaypoint::UiWaypoint(Backlight* backlight) : UiBase(), backlight_(backlight) {
  static lv_style_t container_style;
  lv_style_init(&container_style);
  lv_style_set_pad_column(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);
  lv_style_set_flex_flow(&container_style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(&container_style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_flex_cross_place(&container_style, LV_FLEX_ALIGN_CENTER);
  lv_style_set_flex_track_place(&container_style, LV_FLEX_ALIGN_CENTER);
  lv_style_set_layout(&container_style, LV_LAYOUT_FLEX);
  lv_style_set_pad_all(&container_style, 0);
  lv_style_set_pad_column(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);

  distance_label_ = lv_label_create(content_);

  static lv_color_t
      cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

  canvas_ = lv_canvas_create(content_);
  lv_canvas_set_buffer(canvas_, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT,
                       LV_IMG_CF_TRUE_COLOR);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

  lv_obj_add_style(distance_label_, &style, 0);
  lv_obj_set_flex_grow(distance_label_, 1);
}

void UiWaypoint::Update(GpsInfo gps_info) {
  char title_text[15];
  if (mode_ == Mode::kWaypoints) {
    backlight_->SetColor(255, 0, 0, 0);
    if (current_waypoint_index_ == 0) {
      snprintf(title_text, 15, "WAYPOINT CAMP");
    } else {
      snprintf(title_text, 15, "WAYPOINT %d", current_waypoint_index_);
    }
  } else {
    backlight_->SetColor(255, 0, 0, 100);
    snprintf(title_text, 15, "BATHROOM");
  }
  SetTitle(title_text);

  gps_info_ = gps_info;

  if (!gps_info.valid) {
    lv_label_set_text(distance_label_, "No\nGPS");
    draw_circle();
    return;
  }

  MaybeValid<LatLon> waypoint;
  if (mode_ == Mode::kWaypoints) {
    waypoint = ws_.Read(current_waypoint_index_);
    if (!waypoint.valid) {
      lv_label_set_text(distance_label_, "Not\nSet");
      draw_circle();
      return;
    }
  } else {
    waypoint = MakeValid<LatLon>(
        FindNearestBathroom({.lat = gps_info.lat, .lon = gps_info.lon}));
  }

  // Display distance.
  char distance_text[20];
  const double distance_ft = metersToFeet(distanceBetween(
      waypoint.value, {.lat = gps_info.lat, .lon = gps_info.lon}));
  if (distance_ft <= 9999.0) {
    snprintf(distance_text, 20, "%d\nft", (int)distance_ft);
  } else {
    snprintf(distance_text, 20, "%.1f\nmi", feetToMiles(distance_ft));
  }
  lv_label_set_text(distance_label_, distance_text);

  // Compute course required to get to waypoint.
  const double course_to_waypoint =
      courseTo(waypoint.value, {.lat = gps_info.lat, .lon = gps_info.lon});
  double course_diff = course_to_waypoint - gps_info.course_deg;
  if (course_diff > 360.0)
    course_diff -= 360.0;
  else if (course_diff < -360.0)
    course_diff += 360.0;

  if (distance_ft < 40.0 || gps_info_.speed_mph < 2.0) {
    // Direction information is not very stable if waypoint is very close or if
    // speed is low.
    draw_circle();
  } else {
    draw_arrow(course_diff);
  }
}

void UiWaypoint::IncrementWaypoint() {
  if (mode_ != Mode::kWaypoints) return;
  current_waypoint_index_++;
  if (current_waypoint_index_ >= kMaxTotalWaypoints)
    current_waypoint_index_ = 0;
}

void UiWaypoint::SaveWaypoint() {
  if (mode_ != Mode::kWaypoints) return;
  if (!gps_info_.valid) return;
  ws_.Write(current_waypoint_index_,
            {.lat = gps_info_.lat, .lon = gps_info_.lon});
}
