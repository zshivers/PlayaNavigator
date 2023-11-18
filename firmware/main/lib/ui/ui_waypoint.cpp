#include "ui_waypoint.h"

#include <cmath>
#include <limits>

#include "conversions.h"
#include "coordinates.h"
#include "lvgl.h"
#include "waypoint_storage.h"

namespace {

constexpr double kMinDistanceToShowDirection_m = 9;
constexpr double kDirectionEstimationMinDistanceChange_m = 3.0;

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

LatLon FindNearestBathroom(const std::vector<LatLon>& bathrooms,
                           LatLon current_location) {
  double closest_distance = std::numeric_limits<double>::infinity();
  LatLon closest_bathroom;
  for (const LatLon bathroom : bathrooms) {
    const double distance = distanceBetween(current_location, bathroom);
    if (distance < closest_distance) {
      closest_distance = distance;
      closest_bathroom = bathroom;
    }
  }
  return closest_bathroom;
}
}  // namespace

// Draw a thick arrow to show direction to waypoint.
// Angle convention:
//   0 = Up
//  90 = Right
// 180 = Down
// 270 = Left
void UiWaypoint::DrawDirectionArrow(float angle_deg) {
  lv_canvas_fill_bg(arrow_canvas_, lv_color_white(), LV_OPA_COVER);

  static lv_draw_rect_dsc_t dsc;
  lv_draw_rect_dsc_init(&dsc);
  dsc.bg_color = lv_color_black();

  constexpr lv_point_t kCanvasCenter = {kCanvasWidth / 2, kCanvasHeight / 2};

  // Since the LVGL library can only draw convex shapes, split the arrow into
  // two parts: The triangle (tip of the arrow) and a rectancgle (base of the
  // arrow).
  lv_point_t tip_points[] = {
      {14, 0},
      {0, 22},
      {28, 22},
  };
  translate_points(tip_points, 3, {6, 0});
  rotate_points(tip_points, 3, kCanvasCenter, angle_deg);
  lv_canvas_draw_polygon(arrow_canvas_, tip_points, 3, &dsc);

  lv_point_t base_points[] = {
      {8, 20},
      {8, 40},
      {20, 40},
      {20, 20},
  };
  translate_points(base_points, 4, {6, 0});
  rotate_points(base_points, 4, kCanvasCenter, angle_deg);
  lv_canvas_draw_polygon(arrow_canvas_, base_points, 4, &dsc);
}

// Draw a filled-in circle in the middle of the canvas.
void UiWaypoint::DrawCircle() {
  static lv_draw_arc_dsc_t dsc;
  lv_draw_arc_dsc_init(&dsc);
  dsc.color = lv_color_black();
  dsc.width = 15;

  lv_canvas_fill_bg(arrow_canvas_, lv_color_white(), LV_OPA_COVER);
  // The only way to draw a circle on a canvas is to draw an arc with a thick
  // border.
  lv_canvas_draw_arc(arrow_canvas_, kCanvasWidth / 2, kCanvasHeight / 2, 15, 0,
                     360, &dsc);
}

UiWaypoint::UiWaypoint(const PlayaMapConfig& map_config,
                       const std::vector<LatLon>& bathrooms)
    : UiBase(), map_config_(map_config), bathrooms_(bathrooms) {
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

  arrow_canvas_ = lv_canvas_create(content_);
  lv_canvas_set_buffer(arrow_canvas_, cbuf, kCanvasWidth, kCanvasHeight,
                       LV_IMG_CF_TRUE_COLOR);

  address_label_ = lv_label_create(window_);
  lv_obj_set_size(address_label_, LV_PCT(100), 11);
  lv_obj_set_flex_flow(address_label_, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(address_label_, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_label_set_text(address_label_, "");

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  // lv_style_set_border_width(&style, 1);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

  lv_obj_add_style(distance_label_, &style, 0);
  lv_obj_set_flex_grow(distance_label_, 1);

  static lv_style_t address_style;
  lv_style_init(&address_style);
  lv_style_set_radius(&address_style, 0);
  lv_style_set_border_color(&address_style, lv_color_black());
  // lv_style_set_border_width(&address_style, 1);
  lv_style_set_text_font(&address_style, &lv_font_unscii_8);
  lv_style_set_text_align(&address_style, LV_TEXT_ALIGN_CENTER);

  lv_obj_add_style(address_label_, &address_style, 0);
}

void UiWaypoint::Update(GpsInfo gps_info) {
  if (mode_ == Mode::kWaypoints) {
    SetTitle(waypoint_names_[current_waypoint_index_]);
  } else {
    SetTitle("BATHROOM");
  }

  gps_info_ = gps_info;

  if (!gps_info.valid) {
    lv_label_set_text(distance_label_, "No\nGPS");
    DrawCircle();
    UpdateAddressText(MakeInvalid<LatLon>({}));
    return;
  }

  MaybeValid<LatLon> waypoint;
  if (mode_ == Mode::kWaypoints) {
    waypoint = ws_.Read(current_waypoint_index_);
    if (!waypoint.valid) {
      lv_label_set_text(distance_label_, "Not\nSet");
      DrawCircle();
      lv_label_set_text(address_label_, "Prs&Hold to Set");
      return;
    }
  } else {
    waypoint =
        MakeValid<LatLon>(FindNearestBathroom(bathrooms_, gps_info.location));
  }

  // Display distance.
  char distance_text[20];
  const double distance_ft =
      metersToFeet(distanceBetween(waypoint.value, gps_info.location));
  if (distance_ft <= 9999.0) {
    snprintf(distance_text, sizeof(distance_text), "%d\nft",
             static_cast<int>(distance_ft));
  } else if (distance_ft <= milesToFeet(1000.0)) {
    snprintf(distance_text, sizeof(distance_text), "%.1f\nmi",
             feetToMiles(distance_ft));
  } else {
    snprintf(distance_text, sizeof(distance_text), "%.0f\nmi",
             feetToMiles(distance_ft));
  }
  lv_label_set_text(distance_label_, distance_text);

  // Compute course required to get to waypoint.
  EstimateDirection(gps_info);
  if (course_deg_.valid) {
    const double course_to_waypoint =
        courseTo(gps_info.location, waypoint.value);
    double course_diff = course_to_waypoint - course_deg_.value;
    // std::cout << "Course to Waypoint: " << course_to_waypoint << " , Course:
    // " << course_deg_.value << "\n";
    if (course_diff > 360.0)
      course_diff -= 360.0;
    else if (course_diff < -360.0)
      course_diff += 360.0;

    if (distance_ft < kMinDistanceToShowDirection_m) {
      // Direction information is not very stable if waypoint is very close.
      // Just show a circle instead of a direction arrow.
      DrawCircle();
    } else {
      DrawDirectionArrow(course_diff);
    }
  } else {
    // No course information is available.
    DrawCircle();
  }

  // TODO optimize so this isn't called as much.
  UpdateAddressText(waypoint);
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
  ws_.Write(current_waypoint_index_, gps_info_.location);
}

void UiWaypoint::UpdateAddressText(MaybeValid<LatLon> waypoint) {
  if (!waypoint.valid) {
    lv_label_set_text(address_label_, "");
    return;
  }
  const PlayaAddress address = LatLonToAddress(map_config_, waypoint.value);
  char text[50];
  if (address.road.valid) {
    // If on a road, show radial and the road letter.
    snprintf(text, sizeof(text), "%02d:%02d & %c", address.hour, address.minute,
             address.road.value);
  } else {
    // If not on a road, show radial and distance from man. Switch between feet
    // and miles.
    const double feetToMan = metersToFeet(address.radius_m);
    if (feetToMan <= 9999.0) {
      snprintf(text, sizeof(text), "%02d:%02d & %d ft", address.hour,
               address.minute, static_cast<int>(feetToMan));
    } else {
      const double milesToMan = metersToMiles(address.radius_m);
      snprintf(text, sizeof(text), "%02d:%02d & %.1f mi", address.hour,
               address.minute, milesToMan);
    }
  }
  lv_label_set_text(address_label_, text);
}

void UiWaypoint::EstimateDirection(const GpsInfo& gps_info) {
  if (!gps_info.valid) return;
  // TODO reset the direction start and end points if GPS is invalid for too
  // long.

  if (!dir_initial.valid) {
    // If this is the first valid GPS point, record it as the initial direction
    // point.
    dir_initial = MakeValid<LatLon>(gps_info.location);
  } else if (!dir_end.valid &&
             distanceBetween(dir_initial.value, gps_info.location) >
                 kDirectionEstimationMinDistanceChange_m) {
    dir_end = MakeValid<LatLon>(gps_info.location);
  }

  if (dir_initial.valid && dir_end.valid) {
    if (distanceBetween(dir_end.value, gps_info.location) >
        kDirectionEstimationMinDistanceChange_m) {
      dir_initial = MakeValid<LatLon>(dir_end.value);
      dir_end = MakeValid<LatLon>(gps_info.location);
    }
    course_deg_ = MakeValid<double>(courseTo(dir_initial.value, dir_end.value));
  } else {
    course_deg_ = MakeInvalid<double>(0.0);
  }
}