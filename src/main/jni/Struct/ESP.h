#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <math.h>

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Bools.h"
#include "Class.h"
#include "Logger.h"
#include "Rect.h"
#include "Unity/MonoString.h"
#include "Unity/unity.h"
#include "obfuscate.h"

// ============================================================================
// ESP OPTIMIZATION - NAME CACHING SYSTEM
// ============================================================================

struct CachedName {
  std::string name;
  uint64_t    timestamp;
  int         originalLength;
};

// Cache for player names to avoid repeated UTF-8 conversion
static std::unordered_map<void *, CachedName> nameCache;
static std::mutex                             nameCacheMutex;
static const uint64_t                         CACHE_TIMEOUT_MS = 60 * 1000;

// Optimized name retrieval with caching
std::string GetCachedPlayerName(void *player) {
  if (!player) return "";

  try {
    std::lock_guard<std::mutex> lock(nameCacheMutex);

    auto now =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

    // Check if we have cached name
    auto it = nameCache.find(player);
    if (it != nameCache.end()) {
      // Check if cache is still valid
      if (now - it->second.timestamp < CACHE_TIMEOUT_MS) {
        return it->second.name;
      }
    }

    // Get fresh name
    std::string playerName;
    try {
      MonoString *Nick = get_NickName(player);
      if (!Nick) return "";

      int nick_Len = Nick->getLength();
      if (nick_Len <= 0 || nick_Len >= 1000) return "";

      playerName = get_UTF8_String_Safe(Nick);

      // Cache the result
      nameCache[player] = {playerName, static_cast<uint64_t>(now), nick_Len};
    } catch (...) {
      return "";
    }

    return playerName;
  } catch (...) {
    return "";
  }
}

// Clean expired cache entries (call periodically)
void CleanNameCache() {
  try {
    std::lock_guard<std::mutex> lock(nameCacheMutex);

    auto now =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

    for (auto it = nameCache.begin(); it != nameCache.end();) {
      if (now - it->second.timestamp > CACHE_TIMEOUT_MS) {
        it = nameCache.erase(it);
      } else {
        ++it;
      }
    }
  } catch (...) {
  }
}

// Utility functions
bool isOutsideScreen(ImVec2 pos, ImVec2 screen) {
  return (pos.y < 0 || pos.x > screen.x || pos.y > screen.y || pos.x < 0);
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset) {
  int x = (int)Pos.x;
  int y = (int)Pos.y;

  if (Pos.y < 0) y = -offset;
  if (Pos.x > screen.x) x = (int)screen.x + offset;
  if (Pos.y > screen.y) y = (int)screen.y + offset;
  if (Pos.x < 0) x = -offset;

  return ImVec2(x, y);
}

// Drawing functions
void drawText2(int x, int y, long color, const char *str, float size) {
  ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
}

void DrawRectFilled(int x, int y, int w, int h, ImColor color) {
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawRectFilledHealth(int x, int y, int w, int h, ImVec4 color) {
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                                ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, int T) {
  ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), color);
}

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius) {
  if (max_health <= 0) return;

  float a_max       = (3.14159265359f * 2.0f);
  ImU32 healthColor = IM_COL32(45, 180, 45, 255);

  float healthPercent = (float)health / max_health;
  if (healthPercent <= 0.6f) healthColor = IM_COL32(180, 180, 45, 255);
  if (healthPercent < 0.3f) healthColor = IM_COL32(180, 45, 45, 255);

  ImGui::GetForegroundDrawList()->PathArcTo(
      position, radius, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
  ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

// Visibility check
bool IsVisible(void *player) {
  if (player != NULL) {
    void   *hitObj         = NULL;
    Vector3 cameraLocation = Transform_GetPosition(Component_GetTransform(Camera_main()));
    Vector3 headLocation   = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(player)));
    return !Physics_Raycast(cameraLocation, headLocation, 12, &hitObj);
  }
  return false;
}

// Health display
void DrawHealths(Rect box, float entityHealth, float maxHealth, int TeamID, const char *name, long color) {
  if (!name || maxHealth <= 0) return;

  float x = box.x - (140 - box.w) * 0.5f;
  float y = box.y;

  // Cache string length and create TeamText
  int  nameLen = strlen(name);
  char TeamText[8];
  snprintf(TeamText, sizeof(TeamText), "%d", TeamID);

  // Simplified health color calculation
  float  healthRatio = entityHealth / maxHealth;
  ImVec4 HPColor     = (healthRatio < 0.3f)   ? ImVec4{0.5f, 0.0f, 0.0f, 0.5f}
                       : (healthRatio < 0.6f) ? ImVec4{1.0f, 0.0f, 0.0f, 0.5f}
                                              : ImVec4{1.0f, 1.0f, 0.0f, 0.5f};

  // Draw background rectangles
  DrawRectFilled(x - nameLen, y - 41, 120 + nameLen * 2, 20, color);
  DrawRectFilled(x - nameLen, y - 41, 25, 20, color);

  // Draw team text
  int teamTextX = (strlen(TeamText) < 2) ? x + 6 - nameLen : x - nameLen;
  drawText2(teamTextX, y - 42, ImColor(255, 255, 255), TeamText, 18.943f);

  // Draw player name
  drawText2(x + 28 - nameLen, y - 43, ImColor(255, 255, 255), name, 18.943f);

  // Draw health bar
  const float maxWidth       = 120.0f;
  float       healthBarWidth = entityHealth * maxWidth / maxHealth;
  DrawRectFilledHealth(x - maxWidth * 0.5f, y - 18, healthBarWidth, 8, HPColor);

  // Draw triangle
  float centerX = box.x + box.w * 0.5f;
  DrawTriangle(centerX - 10, y - 8, centerX + 5, y - 8, centerX - 2, y, ImColor(255, 255, 255), 1);
}

// Get all enemies
std::vector<void *> GetAllEnemies() {
  std::vector<void *> enemies;

  try {
    void *get_MatchGame = Curent_Match();
    if (!get_MatchGame) return enemies;

    void *LocalPlayer = GetLocalPlayer(get_MatchGame);
    if (!LocalPlayer) return enemies;

    monoDictionary<uint8_t *, void **> *players =
        *(monoDictionary<uint8_t *, void **> **)((uintptr_t)get_MatchGame + ListPlayer);
    if (!players) return enemies;

    enemies.reserve(16);

    int playerCount = players->getNumValues();
    for (int u = 0; u < playerCount; u++) {
      void *Player = players->getValues()[u];
      if (Player && Player != LocalPlayer && get_isVisible(Player) && !get_isLocalTeam(Player)) {
        enemies.push_back(Player);
      }
    }
  } catch (...) {
    enemies.clear();
  }
  return enemies;
}

// Get closest enemy for aimbot
void *GetClosestEnemy() {
  float minAngle     = 9999;
  float minDistance  = 99999;
  void *closestEnemy = NULL;

  try {
    void *get_MatchGame = Curent_Match();
    if (!get_MatchGame) return NULL;

    void *LocalPlayer = GetLocalPlayer(get_MatchGame);
    if (!LocalPlayer) return NULL;

    std::vector<void *> enemies = GetAllEnemies();
    for (void *Player : enemies) {
      if (!Player || Player == LocalPlayer || get_IsDieing(Player)) continue;
      try {
        Vector3 PlayerPos      = getPosition(Player);
        Vector3 LocalPlayerPos = getPosition(LocalPlayer);
        float   distance       = Vector3::Distance(LocalPlayerPos, PlayerPos);
        if (distance >= g_AimbotConfig->Aimdis) continue;

        // Quick screen check
        Vector3 pos2 = WorldToScreenPoint(Camera_main(), PlayerPos);
        if (!isFov(Vector3(pos2.x, pos2.y), Vector3(g_GlWidth / 2, g_GlHeight / 2), g_AimbotConfig->Fov_Aim)) continue;

        // Simple angle calculation
        Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
        float   angle     = Vector3::Angle(targetDir, GetForward(Component_GetTransform(Camera_main()))) * 100.0f;
        if (angle <= g_AimbotConfig->Fov_Aim) {
          if ((fabs(angle - minAngle) < 1e-3 && distance < minDistance) || angle < minAngle) {
            minAngle     = angle;
            minDistance  = distance;
            closestEnemy = Player;
          }
        }
      } catch (...) {
      }
    }
  } catch (...) {
    return NULL;
  }

  return closestEnemy;
}

// Main ESP and Aimbot function
inline void DrawESP(float screenWidth, float screenHeight) {
  ImDrawList *draw = ImGui::GetBackgroundDrawList();
  if (!draw) return;

  static int  frameCount  = 0;
  static auto lastCleanup = std::chrono::steady_clock::now();

  if (++frameCount >= 300) {
    auto now                  = std::chrono::steady_clock::now();
    auto timeSinceLastCleanup = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup).count();

    if (!nameCache.empty() && timeSinceLastCleanup >= 5) {
      CleanNameCache();
      lastCleanup = now;
    }
    frameCount = 0;
  }

  void *CurrentMatch = Curent_Match();
  void *LocalPlayer  = nullptr;

  if (CurrentMatch != nullptr) {
    LocalPlayer = GetLocalPlayer(CurrentMatch);
  }

  // Aimbot logic
  if (g_AimbotConfig->Aimbot && CurrentMatch != nullptr && LocalPlayer != nullptr) {
    void *closestEnemy = GetClosestEnemy();
    if (closestEnemy != nullptr) {
      try {
        Vector3    EnemyLocation  = GetHeadPosition(closestEnemy);
        Vector3    PlayerLocation = CameraMain(LocalPlayer);
        Quaternion PlayerLook     = GetRotationToLocation(EnemyLocation, 0.1f, PlayerLocation);
        bool       IsScopeOn      = get_IsSighting(LocalPlayer);
        bool       IsFiring       = get_IsFiring(LocalPlayer);

        if (g_AimbotConfig->AimWhen == 0) {
          set_aim(LocalPlayer, PlayerLook);
        } else if (g_AimbotConfig->AimWhen == 1 && IsFiring) {
          set_aim(LocalPlayer, PlayerLook);
        } else if (g_AimbotConfig->AimWhen == 2 && IsScopeOn) {
          set_aim(LocalPlayer, PlayerLook);
        }
      } catch (...) {
      }
    }
  }

  // ESP logic
  if (g_ESPConfig->ESP_Enable && CurrentMatch != nullptr && LocalPlayer != nullptr) {
    void *camera = Camera_main();
    if (camera != nullptr) {
      // Get all enemies for ESP (no FOV restriction)
      std::vector<void *> enemies = GetAllEnemies();

      int enemiesQuantity = enemies.size();

      ImGui::GetBackgroundDrawList()->AddText(ImVec2(20, 20), ImColor(255, 0, 0),
                                              (std::to_string(enemiesQuantity)).c_str());

      // ESP_FOVCircle
      if (g_AimbotConfig->Aimbot) {
        draw->AddCircle(ImVec2(screenWidth / 2, screenHeight / 2), g_AimbotConfig->Fov_Aim, ImColor(255, 255, 255), 0,
                        1.5f);
      }

      for (void *enemy : enemies) {
        // Get positions once and reuse
        Vector3 Toepos  = getPosition(enemy);
        Vector3 Toeposi = WorldToScreenPoint(camera, Toepos);
        if (Toeposi.z < 1) continue;

        Vector3 HeadPos      = Toepos + Vector3(0, 1.9f, 0);
        Vector3 HeadPosition = WorldToScreenPoint(camera, HeadPos);
        if (HeadPosition.z < 1) continue;

        // Calculate rect and distance
        float height = abs(HeadPosition.y - Toeposi.y) * 1.2f;
        float width  = height * 0.5f;
        Rect  rect   = Rect(HeadPosition.x - width * 0.5f, screenHeight - HeadPosition.y, width, height);

        // Quick screen bounds check (simplified)
        if (rect.x > screenWidth || rect.y > screenHeight || rect.x + rect.w < 0 || rect.y + rect.h < 0) continue;

        float distance = Vector3::Distance(getPosition(LocalPlayer), Toepos);

        // Line ESP
        if (g_ESPConfig->ESP_Line) {
          draw->AddLine(ImVec2(screenWidth / 2, 80), ImVec2(rect.x + rect.w / 2, rect.y + rect.h / 35),
                        ImColor(255, 255, 255), 1.7);
        }

        // Box ESP
        if (g_ESPConfig->ESP_Box) {
          draw->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.w, rect.y + rect.h), ImColor(255, 255, 255),
                        visual_esp_box, 0, visual_esp_boxth);
        }

        // Health ESP
        if (g_ESPConfig->ESP_Health) {
          float maxHP     = get_MaxHP(enemy);
          float currentHP = GetHp(enemy);

          if (maxHP > 0 && currentHP > 0 && currentHP <= maxHP) {
            // Simplified color logic
            long clr = (currentHP < maxHP * 0.3f)    ? ImColor(255, 0, 0, 255)
                       : (currentHP <= maxHP * 0.6f) ? ImColor(255, 255, 0, 255)
                                                     : ImColor(0, 255, 0, 255);

            if (get_IsDieing(enemy)) clr = die;

            int xx = rect.x + rect.w + 2;
            int yy = rect.y;
            draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 5, yy + rect.h), ImColor(0, 0, 0, 255));
            draw->AddRectFilled(ImVec2(xx + 1, yy + rect.h - (rect.h * (currentHP / maxHP))),
                                ImVec2(xx + 4, yy + rect.h), clr);
          }
        }

        // Name and Distance ESP
        if (g_ESPConfig->ESP_Name || g_ESPConfig->ESP_Distance) {
          std::string displayText;
          if (g_ESPConfig->ESP_Distance) displayText = "[" + int_to_string((int)distance) + "] ";
          if (g_ESPConfig->ESP_Name) {
            std::string playerName = GetCachedPlayerName(enemy);
            if (!playerName.empty()) displayText += playerName;
          }

          if (!displayText.empty()) {
            ImVec2  textSize  = ImGui::CalcTextSize(displayText.c_str());
            ImColor textColor = get_IsDieing(enemy) ? ImColor(255, 0, 0) : ImColor(255, 255, 0);
            draw->AddText(ImVec2(rect.x + (rect.w - textSize.x) * 0.5f, rect.y - textSize.y), textColor,
                          displayText.c_str());
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