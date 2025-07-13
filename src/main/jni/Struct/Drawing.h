//
// Created by Rev on 29/05/2021.
//

#ifndef ImGuiAndroid_Drawing
#define ImGuiAndroid_Drawing

#include <../Unity/Color.h>

#include <algorithm>

#include "../ImGui/imgui_internal.h"
#include "Rect.h"
#include "autohelp.h"

namespace Drawing {
// Helper function to clamp values
inline float clamp(float value, float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

// Helper function to convert Color to ImU32
ImU32 ColorToImU32(const Color &color) {
  // Clamp values to [0, 1] range and convert to [0, 255]
  int r = (int)(clamp(color.r, 0.0f, 1.0f) * 255);
  int g = (int)(clamp(color.g, 0.0f, 1.0f) * 255);
  int b = (int)(clamp(color.b, 0.0f, 1.0f) * 255);
  int a = (int)(clamp(color.a, 0.0f, 1.0f) * 255);

  return ImColor(r, g, b, a);
}

void ImDrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow) {
  ImDrawList *windowDrawList = ImGui::GetWindowDrawList();

  Color colColor(0, 0, 0);

  flRainbow += flSpeed;
  if (flRainbow > 1.f) flRainbow = 0.f;

  for (int i = 0; i < width; i++) {
    float hue = (1.f / (float)width) * i;
    hue -= flRainbow;
    if (hue < 0.f) hue += 1.f;

    Color colRainbow = FromHSB(hue, 1.f, 1.f);
    windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ColorToImU32(colRainbow));
  }
}

void ImDrawRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow) {
  auto windowDrawList = ImGui::GetBackgroundDrawList();

  Color colColor(0, 0, 0);

  flRainbow += flSpeed;
  if (flRainbow > 1.f) flRainbow = 0.f;

  for (int i = 0; i < width; i++) {
    float hue = (1.f / (float)width) * i;
    hue -= flRainbow;
    if (hue < 0.f) hue += 1.f;

    Color colRainbow = FromHSB(hue, 1.f, 1.f);
    windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ColorToImU32(colRainbow));
  }
}

void DrawLine(ImVec2 start, ImVec2 end, ImVec4 color) {
  auto background = ImGui::GetBackgroundDrawList();

  if (background) {
    background->AddLine(start, end, ImColor(color.x, color.y, color.z, color.w));
  }
}

void DrawWatermark1(ImVec2 pos, ImVec2 stroke, ImVec4 color) {
  auto background = ImGui::GetBackgroundDrawList();

  if (background) {
    background->AddRectFilled(pos, stroke, ImColor(color.x, color.y, color.z, color.w));
  }
}

void DrawBox(Rect rect, ImVec4 color) {
  ImVec2 v1(rect.x, rect.y);
  ImVec2 v2(rect.x + rect.w, rect.y);
  ImVec2 v3(rect.x + rect.w, rect.y + rect.h);
  ImVec2 v4(rect.x, rect.y + rect.h);

  DrawLine(v1, v2, color);
  DrawLine(v2, v3, color);
  DrawLine(v3, v4, color);
  DrawLine(v4, v1, color);
}

void DrawCircle(float X, float Y, float radius, bool filled, ImVec4 color) {
  auto background = ImGui::GetBackgroundDrawList();

  if (background) {
    if (filled) {
      background->AddCircleFilled(ImVec2(X, Y), radius, ImColor(color.x, color.y, color.z, color.w));
    }

    else {
      background->AddCircle(ImVec2(X, Y), radius, ImColor(color.x, color.y, color.z, color.w));
    }
  }
}

void DrawText2(float fontSize, ImVec2 position, ImVec4 color, const char *text) {
  auto background = ImGui::GetBackgroundDrawList();

  if (background) {
    background->AddText(NULL, fontSize, position, ImColor(color.x, color.y, color.z, color.w), text);
  }
}
}  // namespace Drawing

#endif
