
#include "usb_storage.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <hardware/flash.h>

#include "FlashIAPBlockDevice.h"
#include "PluggableUSBMSD.h"
#include "coordinates.h"
#include "mbed.h"

namespace {

// 2023 map.
// Manually entered using the info from:
// https://bm-innovate.s3.amazonaws.com/2023/2023%20BRC%20Measurements.pdf
constexpr PlayaMapConfig kPlayaMapConfig2023 = {
    .name = {'B', 'M', ' ', '2', '0', '2', '3',
             '\0'},  // Annoying hack due to compiler issue.
    .center = {.lat = 40.786400, .lon = -119.203500},
    // True north/south line is along 4:30 = (360 deg / 12 h) * (6 - 4.5) = 45
    // deg
    .rotation_deg = -45.0,
    .roads = {{
        {'S', feetToMeters(2500)},
        {'A', feetToMeters(2940)},
        {'B', feetToMeters(3230)},
        {'C', feetToMeters(3520)},
        {'D', feetToMeters(3810)},
        {'E', feetToMeters(4100)},
        {'F', feetToMeters(4590)},
        {'G', feetToMeters(4880)},
        {'H', feetToMeters(5170)},
        {'I', feetToMeters(5460)},
        {'J', feetToMeters(5650)},
        {'K', feetToMeters(5845)},
    }},
};

// 2023 bathroom locations.
// Generated from GeoJSON published by BMorg.
// https://innovate.burningman.org/datasets-page/
// 'Portable Toilets.json'
constexpr std::array<LatLon, 42> kBathroomLocations2023 = {{
    {40.78148727096226, -119.19194430573732},
    {40.779623335561965, -119.18768876029635},
    {40.779227875508674, -119.19324827130822},
    {40.774505958429316, -119.19424899119754},
    {40.77781033547896, -119.19670917967723},
    {40.776713347911084, -119.19982522720662},
    {40.773153181745364, -119.19858504242137},
    {40.77183663489247, -119.2032377661243},
    {40.776377593343945, -119.20325124916008},
    {40.77661618107038, -119.20667967783257},
    {40.77301545763877, -119.20794593511167},
    {40.77595215023015, -119.21685849753761},
    {40.77434534372092, -119.21234726209157},
    {40.779178283335064, -119.2136826798792},
    {40.77766451998434, -119.20993960120644},
    {40.77956187184632, -119.2192078534556},
    {40.78141379878998, -119.21500471761742},
    {40.78287082583834, -119.22104752085453},
    {40.7838583838104, -119.2162560939506},
    {40.78644066821201, -119.22269496900654},
    {40.78649644215529, -119.21673533421647},
    {40.78909612664214, -119.21624761795569},
    {40.790052619902575, -119.22099592332385},
    {40.79151133003912, -119.2148913303925},
    {40.79336804467358, -119.21915307423598},
    {40.796773904573875, -119.21698097178694},
    {40.794120819508834, -119.2129960303536},
    {40.795106748537705, -119.20997267541472},
    {40.79838100545868, -119.2124639257767},
    {40.776166866818144, -119.18985236539424},
    {40.78355564887915, -119.185977616956},
    {40.78492460674579, -119.19274810451302},
    {40.79455620547152, -119.2054689007407},
    {40.79969634524306, -119.20729579321585},
    {40.80223931869832, -119.20345245039532},
    {40.80168897960674, -119.19805597840232},
    {40.801018270167525, -119.19547332421236},
    {40.80361808562586, -119.19169994803791},
    {40.79552056179866, -119.18100419248846},
    {40.791352686639506, -119.195999823131},
    {40.78938326092272, -119.20671617278813},
    {40.78395545642809, -119.19954685881262},
}};

// Total RP2040 Flash memory size = 2048 KiB
constexpr int kFlashTotalBytes = 2048 * 1024;
constexpr int kFlashOffset = 1024 * 1024;
// Set to half the flash size. If set to other values, the filesystem reformat()
// would not work, and the drive would not be recognized.
// TODO - debug the problem and reduce the size of the disk.
constexpr int kUsbFsSizeBytes = 1024 * 1024;
static_assert(kFlashOffset + kUsbFsSizeBytes <= kFlashTotalBytes);
constexpr char kPlayaMapConfigFilename[] = "/fs/map_config.json";

class FileReadAdapter {
 public:
  FileReadAdapter(FILE* f) : f_(f) {}
  int read() { return fgetc(f_); }
  size_t readBytes(char* buffer, size_t length) {
    return fread(buffer, 1, length, f_);
  }

 private:
  FILE* f_;
};

}  // namespace

UsbStorage::UsbStorage()
    : fs_("fs"),
      bd_(XIP_BASE + kFlashOffset, kUsbFsSizeBytes),
      usbmd_(&bd_),
      default_map_config_(kPlayaMapConfig2023) {
  for (size_t i = 0; i < kBathroomLocations2023.size(); ++i)
    bathrooms_.push_back(kBathroomLocations2023[i]);
}

void UsbStorage::Start() {
  int bd_error = bd_.init();
  Serial.println("Block device init() return " + String(bd_error));

  int error = fs_.mount(&bd_);
  if (error) {
    Serial.println("Filesystem mount failed, reformatting");
    int reformat_error = fs_.reformat(&bd_);
    Serial.println("Reformat finished, error = " + String(reformat_error));
  }

  map_file = fopen(kPlayaMapConfigFilename, "r");
  if (map_file == nullptr) {
    Serial.println("Cannot open configuration file");
    fclose(map_file);
    return;
  }

  Serial.println("Opened file");

  FileReadAdapter read_adapter{map_file};

  StaticJsonDocument<10000> doc;
  DeserializationError e = deserializeJson(doc, read_adapter);
  fclose(map_file);

  if (e) {
    Serial.println("JSON deserialization error");
    return;
  }

  strncpy(usb_map_config_.name, doc["map"]["name"],
          sizeof(PlayaMapConfig::name));
  usb_map_config_.center.lat = doc["map"]["center"][0];
  usb_map_config_.center.lon = doc["map"]["center"][1];
  usb_map_config_.rotation_deg = doc["map"]["rotation_deg"];

  for (size_t i = 0; i < doc["map"]["roads"].size(); ++i) {
    const char* road_name = doc["map"]["roads"][i]["name"].as<const char*>();
    usb_map_config_.roads[i].road = road_name[0];
    usb_map_config_.roads[i].radius_m =
        feetToMeters(doc["map"]["roads"][i]["radius_ft"]);
  }

  if (doc["bathrooms"].size() != 0) {
    bathrooms_.clear();  // Erase default bathrooms set in constructor.
    for (size_t i = 0; i < doc["bathrooms"].size(); ++i) {
      bathrooms_.push_back(
          LatLon{.lat = doc["bathrooms"][i][0], .lon = doc["bathrooms"][i][1]});
    }
  }

  usb_config_valid_ = PlayaMapConfigValid(usb_map_config_);
}
