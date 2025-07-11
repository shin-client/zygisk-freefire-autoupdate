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
#include "Logger.h" // Include logging system
#include "obfuscate.h"
#include "Unity/unity.h"
#include "Class.h"
#include "Rect.h"

extern int g_GlWidth, g_GlHeight;

#include "Bools.h"
#include <math.h>
#include <stdio.h>
typedef char PACKAGENAME;

bool isOutsideScreen(ImVec2 pos, ImVec2 screen)
{
  if (pos.y < 0)
  {
    return true;
  }
  if (pos.x > screen.x)
  {
    return true;
  }
  if (pos.y > screen.y)
  {
    return true;
  }
  return pos.x < 0;
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset)
{
  int x = (int)Pos.x;
  int y = (int)Pos.y;

  if (Pos.y < 0)
  {
    y = -offset;
  }

  if (Pos.x > screen.x)
  {
    x = (int)screen.x + offset;
  }

  if (Pos.y > screen.y)
  {
    y = (int)screen.y + offset;
  }

  if (Pos.x < 0)
  {
    x = -offset;
  }
  return ImVec2(x, y);
}

static float transparency = 127 / 255.f;

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius)
{
  float a_max = ((3.14159265359f * 2.0f));
  ImU32 healthColor = IM_COL32(45, 180, 45, 255);
  if (health <= (max_health * 0.6))
  {
    healthColor = IM_COL32(180, 180, 45, 255);
  }
  if (health < (max_health * 0.3))
  {
    healthColor = IM_COL32(180, 45, 45, 255);
  }
  ImGui::GetForegroundDrawList()->PathArcTo(position, radius, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
  ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

void drawText2(int x, int y, long color, const char *str, float size)
{
  ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
}

void DrawRectFilled(int x, int y, int w, int h, ImColor color)
{
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                                color, 0, 0);
}

void DrawRectFilledHealth(int x, int y, int w, int h, ImVec4 color)
{
  ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                                ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, int T)
{
  ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3),
                                                    color);
}
bool IsVisible(void *player)
{
  if (player != NULL)
  {
    void *hitObj = NULL;
    Vector3 cameraLocation = Transform_GetPosition(Component_GetTransform(Camera_main()));
    Vector3 headLocation = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(player)));
    return !Physics_Raycast(cameraLocation, headLocation, 12, &hitObj);
  }
  return false;
}

void DrawHealths(Rect box, float entityHealth, float maxHealth, int TeamID, const char *name, long color)
{
  if (name == NULL)
    return; // Prevent null pointer access

  float x = box.x - (140 - box.w) / 2;
  float y = box.y;
  char dis[20];
  // sprintf(dis, "%0.fM", d);

  char TeamText[32];                                       // Increased buffer size
  snprintf(TeamText, sizeof(TeamText), "%d", (int)TeamID); // Safe sprintf

  ImVec4 HPColor = ImVec4{1, 1, 0, 127 / 255.f}; // Màu mặc định cho máu

  if (entityHealth < maxHealth && maxHealth > 0) // Prevent division by zero
  {
    float healthPercentage = entityHealth / maxHealth;

    if (healthPercentage < 0.3f)
    {
      HPColor = ImVec4{0.5f, 0.0f, 0.0f, 127 / 255.f}; // Màu đỏ cho máu thấp
    }
    else if (healthPercentage < 0.6f)
    {
      HPColor = ImVec4{1, 0, 0, 127 / 255.f}; // Màu cam cho máu trung bình
    }
  }

  transparency = 100.f / 255.f;
  int nameLen = (name != NULL) ? strlen(name) : 0;                   // Safe strlen
  DrawRectFilled(x - nameLen, y - 41, 120 + nameLen * 2, 20, color); // Đám mây nền cho tên người chơi
  transparency = 255.f / 255.f;
  DrawRectFilled(x - nameLen, box.y - 41, 25, 20, color);
  if (strlen(TeamText) < 2)
  {
    drawText2(x + 6 - nameLen, box.y - 42, ImColor(255, 255, 255),
              TeamText, 18.943); // 队伍ID
  }
  else
  {
    drawText2(x - nameLen, box.y - 42, ImColor(255, 255, 255),
              TeamText, 18.943); // 队伍ID
  }
  if (name != NULL) // Check name is not null before drawing
  {
    drawText2(x + 28 - nameLen, y - 43,
              ImColor(255, 255, 255), name, 18.943); // 玩家名称
  }
  float maxWidth = 120;                                                               // Độ dài tối đa của thanh máu
  float healthBarWidth = (maxHealth > 0) ? (entityHealth * maxWidth / maxHealth) : 0; // Safe division
  DrawRectFilledHealth(x - maxWidth / 2, y - 18, healthBarWidth, 8, HPColor);         // cập nhật vị trí và kích thước của thanh máu
  drawText2(x + 125 + nameLen, y - 43, ImColor(255, 255, 255), dis, 18.943);          // 距离显示
  DrawTriangle(box.x + box.w / 2 - 10, y - 8, box.x + box.w / 2 + 15 - 10, y - 8,
               box.x + box.w / 2 - 2, y, ImColor(255, 255, 255), 1); // 第一个左边 第二个右边 第三个底边
}

static float SetFieldOfView = 0, GetFieldOfView = 0;

void DroneView()
{
  if (GetFieldOfView == 0)
  {
    GetFieldOfView = get_fieldOfView();
  }
  if (SetFieldOfView > 0 && GetFieldOfView != 0)
  {
    set_fieldOfView((float)GetFieldOfView + SetFieldOfView);
  }
  if (SetFieldOfView == 0 && GetFieldOfView != 0)
  {
    set_fieldOfView((float)GetFieldOfView);
  }
}

void *GetClosestEnemy()
{
  float shortestDistance = 9999;
  void *closestEnemy = NULL;

  void *get_MatchGame = Curent_Match();
  if (get_MatchGame == NULL)
  {
    LOGW("Curent_Match() returned NULL");
    return NULL;
  }

  void *LocalPlayer = GetLocalPlayer(get_MatchGame);
  if (LocalPlayer == NULL)
  {
    LOGW("GetLocalPlayer() returned NULL");
    return NULL;
  }

  // Safe pointer access with validation
  uintptr_t playersPtr = (uintptr_t)get_MatchGame + ListPlayer;
  if (playersPtr == 0)
  {
    LOGE("Invalid players pointer address");
    return NULL;
  }

  monoDictionary<uint8_t *, void **> *players = *(monoDictionary<uint8_t *, void **> **)playersPtr;
  if (players == NULL)
  {
    LOGW("Players dictionary is NULL");
    return NULL;
  }

  int playerCount = players->getNumValues();
  if (playerCount <= 0)
  {
    LOGD("No players found in dictionary");
    return NULL;
  }

  LOGD("Scanning %d players for closest enemy", playerCount);

  for (int u = 0; u < playerCount; u++)
  {
    // Extra bounds checking
    if (u >= playerCount)
      break;

    void *Player = players->getValues()[u];
    if (Player != NULL && Player != LocalPlayer && !get_isLocalTeam(Player) && !get_IsDieing(Player) && get_isVisible(Player) && get_MaxHP(Player))
    {
      Vector3 PlayerPos = getPosition(Player);
      Vector3 LocalPlayerPos = getPosition(LocalPlayer);
      Vector3 pos2 = WorldToScreenPoint(Camera_main(), PlayerPos);
      bool isFov1 = isFov(Vector3(pos2.x, pos2.y), Vector3(g_GlWidth / 2, g_GlHeight / 2), Fov_Aim);

      float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
      if (distance < Aimdis)
      {
        Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
        float angle = Vector3::Angle(targetDir, GetForward(Component_GetTransform(Camera_main()))) * 100.0;

        if (angle <= Fov_Aim && isFov1 && angle < shortestDistance)
        {
          shortestDistance = angle;
          closestEnemy = Player;
        }
      }
    }
  }
  return closestEnemy;
}

float (*get_deltaTime)();
float smoothSpeedAIM = 5;
void *MyPlayer = NULL;
bool Drone;

const float RADAR_SIZE = 200.0f;           // kích thước của radar
const ImVec2 RADAR_CENTER(100.0f, 100.0f); // tọa độ trung tâm của radar

float x, y;

float enemy1 = 2.0f;

float aimSmoothness = 0.1f;

inline void DrawAIMBOT(float screenWidth, float screenHeight)
{
}

inline void DrawESP(float screenWidth, float screenHeight)
{
  ImDrawList *draw = ImGui::GetBackgroundDrawList();
  if (!draw)
    return;

  if (Aimbot)
  {

    void *CurrentMatch = Curent_Match();
    void *closestEnemy = GetClosestEnemy();
    void *LocalPlayer = GetLocalPlayer(CurrentMatch);

    if (closestEnemy != NULL && LocalPlayer != NULL && CurrentMatch != NULL)
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

  if (ESP_Enable)
  {
    int totalEnemies = 0, totalBots = 0;
    void *current_Match = Curent_Match();

    if (current_Match == nullptr)
      return;

    void *local_player = GetLocalPlayer(current_Match);
    if (local_player == nullptr)
      return;

    // Safe pointer access with validation
    uintptr_t playersPtr = (uintptr_t)current_Match + ListPlayer;
    if (playersPtr == 0)
      return;

    monoDictionary<uint8_t *, void **> *players = *(monoDictionary<uint8_t *, void **> **)playersPtr;
    void *camera = Camera_main();

    if (players != nullptr && camera != nullptr)
    {
      int playerCount = players->getNumValues();
      if (playerCount <= 0)
        return;

      for (int u = 0; u < playerCount; u++)
      {
        // Extra bounds checking
        if (u >= playerCount)
          break;

        void *closestEnemy = players->getValues()[u];
        if (closestEnemy != local_player && closestEnemy != nullptr && get_isVisible(closestEnemy) && !get_isLocalTeam(closestEnemy))
        {

          Vector3 Toepos = getPosition(closestEnemy);
          Vector3 Toeposi = WorldToScreenPoint(camera, Vector3(Toepos.x, Toepos.y, Toepos.z));
          if (Toeposi.z < 1)
            continue;

          Vector3 HeadPos = getPosition(closestEnemy) + Vector3(0, 1.9f, 0);
          Vector3 HeadPosition = WorldToScreenPoint(camera, Vector3(HeadPos.x, HeadPos.y, HeadPos.z));
          if (HeadPosition.z < 1)
            continue;

          // FOV Circle for Aimbot
          if (ESP_FOVCircle)
          {
            draw->AddCircle(ImVec2(screenWidth / 2, screenHeight / 2), Fov_Aim, ImColor(255, 255, 255), 0, 1.5f);
          }

          float distance = Vector3::Distance(getPosition(local_player), Toepos);
          float Hight = abs(HeadPosition.y - Toeposi.y) * (1.2 / 1.1);
          float Width = Hight * 0.50f;
          Rect rect = Rect(HeadPosition.x - Width / 2.f, screenHeight - HeadPosition.y, Width, Hight);

          // Line ESP
          if (ESP_Line)
          {
            draw->AddLine(ImVec2(screenWidth / 2, 80), ImVec2(rect.x + rect.w / 2, rect.y + rect.h / 35), ImColor(255, 255, 255), 1.7);
          }

          // Box ESP
          if (ESP_Box)
          {
            int x = rect.x;
            int y = rect.y;
            draw->AddRect(ImVec2(x, y), ImVec2(x + rect.w, y + rect.h), ImColor(255, 255, 255), visual_esp_box, 0, visual_esp_boxth);
          }

          // Health ESP
          if (ESP_Health)
          {
            float maxHP = get_MaxHP(closestEnemy);
            float currentHP = GetHp(closestEnemy);

            // Validate HP values to prevent crashes
            if (maxHP > 0 && currentHP >= 0 && currentHP <= maxHP)
            {
              if (get_IsDieing(closestEnemy))
              {
                int xx = rect.x + rect.w + 2;
                int yy = rect.y;
                draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 5, yy + rect.h), ImColor(0, 0, 0, 255));
                draw->AddRectFilled(ImVec2(xx + 1, yy + rect.h - (rect.h * (currentHP / maxHP))), ImVec2(xx + 4, yy + rect.h), die);
              }
              else
              {
                long clr = ImColor(0, 255, 0, 255);

                if (currentHP <= (maxHP * 0.6))
                {
                  clr = ImColor(255, 255, 0, 255);
                }

                if (currentHP < (maxHP * 0.3))
                {
                  clr = ImColor(255, 0, 0, 255);
                }
                int xx = rect.x + rect.w + 2;
                int yy = rect.y;
                draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 5, yy + rect.h), ImColor(0, 0, 0, 255));
                draw->AddRectFilled(ImVec2(xx + 1, yy + rect.h - (rect.h * (currentHP / maxHP))), ImVec2(xx + 4, yy + rect.h), clr);
              }
            }
          }

          // Name and Distance ESP
          if (ESP_Name || ESP_Distance)
          {
            monoString *Nick = get_NickName(closestEnemy);
            std::string names;

            if (Nick != NULL)
            {
              int nick_Len = Nick->getLength();
              // Bounds checking for string operations
              if (nick_Len > 0 && nick_Len < 1000) // Reasonable limit
              {
                std::string names = get_UTF8_String(Nick);
              }
            }

            std::string displayText;
            if (ESP_Distance)
            {
              displayText += "[";
              displayText += int_to_string((int)distance).c_str();
              displayText += "] ";
            }
            if (ESP_Name)
            {
              displayText += names;
            }

            if (!displayText.empty())
            {
              if (get_IsDieing(closestEnemy))
              {
                ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
                draw->AddText(ImVec2(rect.x + (rect.w / 2) - (textSize.x / 2), rect.y - textSize.y),
                              ImColor(255, 0, 0), displayText.c_str());
              }
              else
              {
                ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
                draw->AddText(ImVec2(rect.x + (rect.w / 2) - (textSize.x / 2), rect.y - textSize.y),
                              ImColor(255, 255, 0), displayText.c_str());
              }
            }
          }
        }
      }
    }
  }
}

bool (*Bypass)(void *bypass);
bool _Bypass(void *bypass)
{
  return false;
}
