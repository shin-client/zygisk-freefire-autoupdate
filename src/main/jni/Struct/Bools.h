#ifndef BOOLS_H
#define BOOLS_H

#include <cmath>
#include <memory>
#include <string>

#include "../Unity/Vector3.hpp"
#include "imgui.h"

#define DIR_COUNT 3

extern int g_GlWidth, g_GlHeight;

class ESP {
 public:
  bool ESP_Enable   = false;
  bool ESP_Box      = false;
  bool ESP_Line     = false;
  bool ESP_Health   = false;
  bool ESP_Name     = false;
  bool ESP_Distance = false;

  void enableAll() {
    ESP_Enable   = true;
    ESP_Box      = true;
    ESP_Line     = true;
    ESP_Health   = true;
    ESP_Name     = true;
    ESP_Distance = true;
  }

  void disableAll() {
    ESP_Enable   = false;
    ESP_Box      = false;
    ESP_Line     = false;
    ESP_Health   = false;
    ESP_Name     = false;
    ESP_Distance = false;
  }
};

class AimbotClass {
 public:
  int   AimWhen = 1;
  float Fov_Aim = 100.0f;
  float Aimdis  = 200.0f;
  bool  Aimbot  = false;
};

extern std::shared_ptr<ESP>         g_ESPConfig;
extern std::shared_ptr<AimbotClass> g_AimbotConfig;

class OtherFeature {
 public:
  bool AntiReport = false;
  bool ResetGuest = false;

  void resetAll() {
    g_ESPConfig->ESP_Enable   = false;
    g_ESPConfig->ESP_Box      = false;
    g_ESPConfig->ESP_Line     = false;
    g_ESPConfig->ESP_Health   = false;
    g_ESPConfig->ESP_Name     = false;
    g_ESPConfig->ESP_Distance = false;
    g_AimbotConfig->Aimbot    = false;
    g_AimbotConfig->Fov_Aim   = 100.0f;
    g_AimbotConfig->Aimdis    = 200.0f;
    AntiReport                = false;
  }
};

extern std::shared_ptr<OtherFeature> g_OtherConfig;

extern float       visual_esp_box;
extern float       visual_esp_boxth;
extern ImColor     die;
extern ImColor     hp;
extern const char *dir[DIR_COUNT];
std::string        int_to_string(int num);
bool               isFov(Vector3 vec1, Vector3 vec2, int radius);
bool               isInsideFOV(int x, int y, float fov_size);

#endif  // BOOLS_H