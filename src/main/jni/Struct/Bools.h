#ifndef BOOLS_H
#define BOOLS_H

#include <cmath>
#include <string>

#include "Vector3.hpp"
#include "imgui.h"

extern int g_GlWidth, g_GlHeight;

// Aimbot Configuration
int   AimWhen = 1;
float Fov_Aim = 100.0f;
float Aimdis  = 200.0f;
bool  Aimbot  = false;

// ESP Configuration
bool ESP_Enable    = false;
bool ESP_Box       = false;
bool ESP_Line      = false;
bool ESP_Health    = false;
bool ESP_Name      = false;
bool ESP_Distance  = false;
bool ESP_FOVCircle = false;

// Anti-Report Feature
bool AntiReport = false;

// Visual Settings
float visual_esp_box   = 1;
float visual_esp_boxth = 1;

// Colors
ImColor die = ImColor(255, 0, 0);
ImColor hp  = ImColor(0, 255, 0, 255);

// Aimbot modes
const char *dir[] = {"None", "Aim", "Scope"};

// Utility functions
std::string int_to_string(int num) {
  return std::to_string(num);
}

bool isFov(Vector3 vec1, Vector3 vec2, int radius) {
  int x  = vec1.x;
  int y  = vec1.y;
  int x0 = vec2.x;
  int y0 = vec2.y;

  return (pow(x - x0, 2) + pow(y - y0, 2)) <= pow(radius, 2);
}

bool isInsideFOV(int x, int y, float fov_size) {
  if (fov_size <= 0) return true;

  int circle_x = g_GlWidth / 2;
  int circle_y = g_GlHeight / 2;
  int rad      = fov_size;

  return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

#endif  // BOOLS_H