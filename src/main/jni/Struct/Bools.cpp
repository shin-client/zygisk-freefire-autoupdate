#include "Bools.h"

std::shared_ptr<ESP>          g_ESPConfig    = std::make_shared<ESP>();
std::shared_ptr<AimbotClass>  g_AimbotConfig = std::make_shared<AimbotClass>();
std::shared_ptr<OtherFeature> g_OtherConfig  = std::make_shared<OtherFeature>();

int         g_GlWidth = 0, g_GlHeight = 0;
float       visual_esp_box = 1, visual_esp_boxth = 1;
ImColor     die = ImColor(255, 0, 0), hp = ImColor(0, 255, 0, 255);
const char *dir[DIR_COUNT] = {"None", "Aim", "Scope"};

std::string int_to_string(int num) { return std::to_string(num); }

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