#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <Il2Cpp.h>
#include <Tools.h>
#include <Vector3.hpp>
#include <Color.h>
#include <Struct/main.h>
#include "Logger.h"
#include "obfuscate.h"
#include "Unity/unity.h"
#include "Class.h"
#include "Rect.h"
#include "Bools.h"
#include <math.h>
#include <unordered_map>
#include <chrono>
#include <vector>

extern int g_GlWidth, g_GlHeight;

// ============================================================================
// ESP OPTIMIZATION - NAME CACHING SYSTEM
// ============================================================================

struct CachedName
{
  std::string name;
  uint64_t timestamp;
  int originalLength;
};

// Cache for player names to avoid repeated UTF-8 conversion
static std::unordered_map<void *, CachedName> nameCache;
static const uint64_t CACHE_TIMEOUT_MS = 60 * 1000;

// Optimized name retrieval with caching
std::string GetCachedPlayerName(void *player)
{
  if (!player)
    return "";

  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch())
                 .count();

  // Check if we have cached name
  auto it = nameCache.find(player);
  if (it != nameCache.end())
  {
    // Check if cache is still valid
    if (now - it->second.timestamp < CACHE_TIMEOUT_MS)
    {
      return it->second.name;
    }
  }

  // Get fresh name
  monoString *Nick = get_NickName(player);
  if (!Nick)
    return "";

  int nick_Len = Nick->getLength();
  if (nick_Len <= 0 || nick_Len >= 1000)
    return "";

  std::string playerName = get_UTF8_String_Safe(Nick);

  // Cache the result
  nameCache[player] = {playerName, static_cast<uint64_t>(now), nick_Len};

  return playerName;
}

// Clean expired cache entries (call periodically)
void CleanNameCache()
{
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch())
                 .count();

  for (auto it = nameCache.begin(); it != nameCache.end();)
  {
    if (now - it->second.timestamp > CACHE_TIMEOUT_MS)
    {
      it = nameCache.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

// Utility functions
bool isOutsideScreen(ImVec2 pos, ImVec2 screen)
{
  return (pos.y < 0 || pos.x > screen.x || pos.y > screen.y || pos.x < 0);
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset)
{
  int x = (int)Pos.x;
  int y = (int)Pos.y;

  if (Pos.y < 0)
    y = -offset;
  if (Pos.x > screen.x)
    x = (int)screen.x + offset;
  if (Pos.y > screen.y)
    y = (int)screen.y + offset;
  if (Pos.x < 0)
    x = -offset;

  return ImVec2(x, y);
}

// Drawing functions
void drawText2(int x, int y, long color, const char *str, float size)
{
  ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
}

void DrawRectFilled(int x, int y, int w, int h, ImColor color)
{
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawRectFilledHealth(int x, int y, int w, int h, ImVec4 color)
{
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                                ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, int T)
{
  ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), color);
}

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius)
{
  if (max_health <= 0)
    return;

  float a_max = (3.14159265359f * 2.0f);
  ImU32 healthColor = IM_COL32(45, 180, 45, 255);

  float healthPercent = (float)health / max_health;
  if (healthPercent <= 0.6f)
    healthColor = IM_COL32(180, 180, 45, 255);
  if (healthPercent < 0.3f)
    healthColor = IM_COL32(180, 45, 45, 255);

  ImGui::GetForegroundDrawList()->PathArcTo(position, radius,
                                            (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health),
                                            a_max - (a_max / 4.0f));
  ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

// Visibility check
bool IsVisible(void *player)
{
  if (player == NULL)
    return false;

  void *hitObj = NULL;
  Vector3 cameraLocation = Transform_GetPosition(Component_GetTransform(Camera_main()));
  Vector3 headLocation = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(player)));
  return !Physics_Raycast(cameraLocation, headLocation, 12, &hitObj);
}

// Health display
void DrawHealths(Rect box, float entityHealth, float maxHealth, int TeamID, const char *name, long color)
{
  if (name == NULL || maxHealth <= 0)
    return;

  float x = box.x - (140 - box.w) / 2;
  float y = box.y;

  char TeamText[32];
  snprintf(TeamText, sizeof(TeamText), "%d", (int)TeamID);

  ImVec4 HPColor = ImVec4{1, 1, 0, 127 / 255.f};

  if (entityHealth < maxHealth)
  {
    float healthPercentage = entityHealth / maxHealth;
    if (healthPercentage < 0.3f)
      HPColor = ImVec4{0.5f, 0.0f, 0.0f, 127 / 255.f};
    else if (healthPercentage < 0.6f)
      HPColor = ImVec4{1, 0, 0, 127 / 255.f};
  }

  int nameLen = strlen(name);
  DrawRectFilled(x - nameLen, y - 41, 120 + nameLen * 2, 20, color);
  DrawRectFilled(x - nameLen, box.y - 41, 25, 20, color);

  if (strlen(TeamText) < 2)
  {
    drawText2(x + 6 - nameLen, box.y - 42, ImColor(255, 255, 255), TeamText, 18.943);
  }
  else
  {
    drawText2(x - nameLen, box.y - 42, ImColor(255, 255, 255), TeamText, 18.943);
  }

  drawText2(x + 28 - nameLen, y - 43, ImColor(255, 255, 255), name, 18.943);

  float maxWidth = 120;
  float healthBarWidth = (entityHealth * maxWidth / maxHealth);
  DrawRectFilledHealth(x - maxWidth / 2, y - 18, healthBarWidth, 8, HPColor);

  DrawTriangle(box.x + box.w / 2 - 10, y - 8, box.x + box.w / 2 + 15 - 10, y - 8,
               box.x + box.w / 2 - 2, y, ImColor(255, 255, 255), 1);
}

// Get closest enemy for aimbot (with FOV restriction)
void *GetClosestEnemy()
{
  float shortestDistance = 9999;
  void *closestEnemy = NULL;
  void *get_MatchGame = Curent_Match();
  void *LocalPlayer = GetLocalPlayer(get_MatchGame);

  if (get_MatchGame == NULL)
    return NULL;

  if (LocalPlayer == NULL)
    return NULL;

  uintptr_t playersPtr = (uintptr_t)get_MatchGame + ListPlayer;
  if (playersPtr == 0)
    return NULL;

  monoDictionary<uint8_t *, void **> *players = *(monoDictionary<uint8_t *, void **> **)playersPtr;
  if (players == NULL)
    return NULL;

  int playerCount = players->getNumValues();
  if (playerCount <= 0)
    return NULL;

  for (int u = 0; u < playerCount; u++)
  {
    if (u >= playerCount)
      break;

    void *Player = players->getValues()[u];
    if (Player == NULL || Player == LocalPlayer || get_isLocalTeam(Player) ||
        get_IsDieing(Player) || !get_isVisible(Player) || !get_MaxHP(Player))
      continue;

    Vector3 PlayerPos = getPosition(Player);
    Vector3 LocalPlayerPos = getPosition(LocalPlayer);
    Vector3 pos2 = WorldToScreenPoint(Camera_main(), PlayerPos);

    float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
    if (distance >= Aimdis)
      continue;

    // FOV check for aimbot only
    bool isFov1 = isFov(Vector3(pos2.x, pos2.y), Vector3(g_GlWidth / 2, g_GlHeight / 2), Fov_Aim);
    if (!isFov1)
      continue;

    Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
    float angle = Vector3::Angle(targetDir, GetForward(Component_GetTransform(Camera_main()))) * 100.0;

    if (angle <= Fov_Aim && angle < shortestDistance)
    {
      shortestDistance = angle;
      closestEnemy = Player;
    }
  }

  return closestEnemy;
}

// Get all enemies for ESP (no FOV restriction)
std::vector<void *> GetAllEnemies()
{
  std::vector<void *> enemies;

  void *get_MatchGame = Curent_Match();
  if (get_MatchGame == NULL)
    return enemies;

  void *LocalPlayer = GetLocalPlayer(get_MatchGame);
  if (LocalPlayer == NULL)
    return enemies;

  uintptr_t playersPtr = (uintptr_t)get_MatchGame + ListPlayer;
  if (playersPtr == 0)
    return enemies;

  monoDictionary<uint8_t *, void **> *players = *(monoDictionary<uint8_t *, void **> **)playersPtr;
  if (players == NULL)
    return enemies;

  int playerCount = players->getNumValues();
  if (playerCount <= 0)
    return enemies;

  for (int u = 0; u < playerCount; u++)
  {
    if (u >= playerCount)
      break;

    void *Player = players->getValues()[u];
    if (Player == NULL || Player == LocalPlayer || get_isLocalTeam(Player) ||
        get_IsDieing(Player) || !get_MaxHP(Player))
      continue;

    // No FOV check for ESP - show all enemies
    enemies.push_back(Player);
  }

  return enemies;
}

// Main ESP and Aimbot function
inline void DrawESP(float screenWidth, float screenHeight)
{
  ImDrawList *draw = ImGui::GetBackgroundDrawList();
  if (!draw)
    return;

  // Smart cache cleanup - only when needed
  static int frameCount = 0;
  static auto lastCleanup = std::chrono::steady_clock::now();

  if (++frameCount >= 300)
  { // Every 5 seconds at 60 FPS
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastCleanup = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup).count();

    // Only cleanup if cache has entries and enough time has passed
    if (!nameCache.empty() && timeSinceLastCleanup >= 5)
    {
      CleanNameCache();
      lastCleanup = now;
    }
    frameCount = 0;
  }

  // Get game data once for both aimbot and ESP
  void *CurrentMatch = Curent_Match();
  void *LocalPlayer = nullptr;

  if (CurrentMatch != nullptr)
  {
    LocalPlayer = GetLocalPlayer(CurrentMatch);
  }

  // Aimbot logic
  if (Aimbot && CurrentMatch != nullptr && LocalPlayer != nullptr)
  {
    void *closestEnemy = GetClosestEnemy();
    if (closestEnemy != nullptr)
    {
      Vector3 EnemyLocation = GetHeadPosition(closestEnemy);
      Vector3 PlayerLocation = CameraMain(LocalPlayer);
      Quaternion PlayerLook = GetRotationToLocation(EnemyLocation, 0.1f, PlayerLocation);
      bool IsScopeOn = get_IsSighting(LocalPlayer);
      bool IsFiring = get_IsFiring(LocalPlayer);

      if (AimWhen == 0)
      {
        set_aim(LocalPlayer, PlayerLook);
      }
      else if (AimWhen == 1 && IsFiring)
      {
        set_aim(LocalPlayer, PlayerLook);
      }
      else if (AimWhen == 2 && IsScopeOn)
      {
        set_aim(LocalPlayer, PlayerLook);
      }
    }
  }

  // ESP logic
  if (ESP_Enable && CurrentMatch != nullptr && LocalPlayer != nullptr)
  {
    void *camera = Camera_main();
    if (camera != nullptr)
    {
      // Get all enemies for ESP (no FOV restriction)
      std::vector<void *> enemies = GetAllEnemies();

      // ESP_FOVCircle
      if (ESP_FOVCircle)
      {
        draw->AddCircle(ImVec2(screenWidth / 2, screenHeight / 2), Fov_Aim, ImColor(255, 255, 255), 0, 1.5f);
      }

      for (void *enemy : enemies)
      {
        if (enemy == nullptr)
          continue;

        Vector3 Toepos = getPosition(enemy);
        Vector3 Toeposi = WorldToScreenPoint(camera, Vector3(Toepos.x, Toepos.y, Toepos.z));
        if (Toeposi.z < 1)
          continue;

        Vector3 HeadPos = getPosition(enemy) + Vector3(0, 1.9f, 0);
        Vector3 HeadPosition = WorldToScreenPoint(camera, Vector3(HeadPos.x, HeadPos.y, HeadPos.z));
        if (HeadPosition.z < 1)
          continue;

        // Check if enemy is within screen bounds for ESP display
        float distance = Vector3::Distance(getPosition(LocalPlayer), Toepos);
        float height = abs(HeadPosition.y - Toeposi.y) * 1.2f;
        float width = height * 0.50f;
        Rect rect = Rect(HeadPosition.x - width / 2.f, screenHeight - HeadPosition.y, width, height);

        // Skip if rect is completely outside screen bounds
        if (rect.x + rect.w < 0 || rect.x > screenWidth || rect.y + rect.h < 0 || rect.y > screenHeight)
          continue;

        // Line ESP
        if (ESP_Line)
        {
          draw->AddLine(ImVec2(screenWidth / 2, 80), ImVec2(rect.x + rect.w / 2, rect.y + rect.h / 35),
                        ImColor(255, 255, 255), 1.7);
        }

        // Box ESP
        if (ESP_Box)
        {
          draw->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.w, rect.y + rect.h),
                        ImColor(255, 255, 255), visual_esp_box, 0, visual_esp_boxth);
        }

        // Health ESP
        if (ESP_Health)
        {
          float maxHP = get_MaxHP(enemy);
          float currentHP = GetHp(enemy);

          if (maxHP > 0 && currentHP >= 0 && currentHP <= maxHP)
          {
            long clr = ImColor(0, 255, 0, 255);
            if (currentHP <= (maxHP * 0.6))
              clr = ImColor(255, 255, 0, 255);
            if (currentHP < (maxHP * 0.3))
              clr = ImColor(255, 0, 0, 255);
            if (get_IsDieing(enemy))
              clr = die;

            int xx = rect.x + rect.w + 2;
            int yy = rect.y;
            draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 5, yy + rect.h), ImColor(0, 0, 0, 255));
            draw->AddRectFilled(ImVec2(xx + 1, yy + rect.h - (rect.h * (currentHP / maxHP))),
                                ImVec2(xx + 4, yy + rect.h), clr);
          }
        }

        // Name and Distance ESP
        if (ESP_Name || ESP_Distance)
        {
          std::string displayText;

          if (ESP_Distance)
          {
            displayText += "[" + int_to_string((int)distance) + "] ";
          }

          if (ESP_Name)
          {
            std::string playerName = GetCachedPlayerName(enemy);
            if (!playerName.empty())
            {
              displayText += playerName;
            }
          }

          if (!displayText.empty())
          {
            ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
            ImColor textColor = get_IsDieing(enemy) ? ImColor(255, 0, 0) : ImColor(255, 255, 0);
            draw->AddText(ImVec2(rect.x + (rect.w / 2) - (textSize.x / 2), rect.y - textSize.y),
                          textColor, displayText.c_str());
          }
        }
      }
    }
  }
}

// Note: Bypass
// bool (*Bypass)(void *bypass);
// bool _Bypass(void *bypass)
// {
//   return false;
// }