#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <limits>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <xdl.h>
#include <KittyUtils.h>
#include <KittyMemory.h>
#include <Il2Cpp.h>
#include <SubstrateHook.h>
#include <CydiaSubstrate.h>
#include "Struct/Gui.hpp"
#include <Struct/main.h>
#include "fonts/FontAwesome6_solid.h"
#include "ImGui/Toggle.h"
#include "zygisk.hpp"
#include "Struct/Class.h"
#include "Struct/obfuscate.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

// Global variables for screen dimensions
int g_GlWidth, g_GlHeight;

// Global log storage - moved from header
std::vector<LogEntry> g_LogBuffer;
bool g_AutoScroll = true;
bool g_LogsVisible = false;
bool g_AutoSaveToFile = true;
std::string g_LogFilePath = "/sdcard/zygisk_ff_logs.txt";
const int MAX_LOG_ENTRIES = 1000;

// Function to get current timestamp
std::string GetCurrentTimestamp()
{
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
  ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
  return ss.str();
}

// Initialize log file
void InitLogFile()
{
  if (!g_AutoSaveToFile)
    return;

  LOGD("Initializing log file: %s", g_LogFilePath.c_str());

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::trunc);
  if (!logFile.is_open())
  {
    // Try alternative paths
    std::vector<std::string> altPaths = {
        "/storage/emulated/0/Download/zygisk_ff_logs.txt",
        "/data/data/com.dts.freefiremax/zygisk_ff_logs.txt",
        "/data/data/com.dts.freefireth/zygisk_ff_logs.txt"};

    for (const auto &altPath : altPaths)
    {
      LOGW("Trying alternative path: %s", altPath.c_str());
      logFile.open(altPath, std::ios::out | std::ios::trunc);
      if (logFile.is_open())
      {
        g_LogFilePath = altPath;
        LOGI("Successfully switched to log path: %s", altPath.c_str());
        break;
      }
    }

    if (!logFile.is_open())
    {
      LOGE("Failed to initialize any log file path");
      g_AutoSaveToFile = false; // Disable auto-save if no path works
      return;
    }
  }

  try
  {
    logFile << "=== ZYGISK FF LOG SESSION START ===" << std::endl;
    logFile << "Session started at: " << GetCurrentTimestamp() << std::endl;
    logFile << "=====================================" << std::endl;
    logFile.flush();
    logFile.close();
    LOGI("Log file initialized successfully");
  }
  catch (...)
  {
    LOGE("Failed to write to log file during initialization");
    logFile.close();
    g_AutoSaveToFile = false;
  }
}

// Write single log entry to file
void WriteLogToFile(const std::string &timestamp, const std::string &level, const std::string &message)
{
  if (!g_AutoSaveToFile)
    return;

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::app);
  if (!logFile.is_open())
  {
    // Try alternative path if main path fails
    std::string altPath = "/data/data/com.dts.freefireth/zygisk_ff_logs.txt";
    logFile.open(altPath, std::ios::out | std::ios::app);
    if (!logFile.is_open())
    {
      return; // Silently fail to avoid spam
    }
    g_LogFilePath = altPath; // Update global path
  }

  try
  {
    logFile << "[" << timestamp << "] [" << level << "] " << message << std::endl;
    logFile.flush(); // Ensure immediate write
    logFile.close();
  }
  catch (...)
  {
    // Silently handle write errors
    logFile.close();
  }
}

// Save all current logs to file
void SaveLogsToFile()
{
  LOGD("Attempting to save logs to file: %s", g_LogFilePath.c_str());

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::trunc);
  if (!logFile.is_open())
  {
    LOGE("Failed to open log file for writing: %s", g_LogFilePath.c_str());

    // Try alternative path
    std::string altPath = "/data/data/com.dts.freefiremax/zygisk_ff_logs.txt";
    LOGW("Trying alternative path: %s", altPath.c_str());

    logFile.open(altPath, std::ios::out | std::ios::trunc);
    if (!logFile.is_open())
    {
      LOGE("Failed to open alternative log file path");
      return;
    }
    g_LogFilePath = altPath; // Update path if successful
    LOGI("Successfully switched to alternative log path");
  }

  try
  {
    logFile << "=== ZYGISK FF COMPLETE LOG DUMP ===" << std::endl;
    logFile << "Generated at: " << GetCurrentTimestamp() << std::endl;
    logFile << "Total entries: " << g_LogBuffer.size() << std::endl;
    logFile << "====================================" << std::endl;

    size_t written = 0;
    for (const auto &entry : g_LogBuffer)
    {
      logFile << "[" << entry.timestamp << "] [" << entry.level << "] " << entry.message << std::endl;
      written++;

      // Flush every 100 entries to ensure data is written
      if (written % 100 == 0)
      {
        logFile.flush();
      }
    }

    logFile << "=== END OF LOG DUMP ===" << std::endl;
    logFile.flush();
    logFile.close();

    LOGI("Successfully saved %zu log entries to file", written);
  }
  catch (const std::exception &e)
  {
    LOGE("Exception occurred while writing logs: %s", e.what());
    logFile.close();
  }
}

// Function to add log entry
void AddLog(const char *level, const char *fmt, ...)
{
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  LogEntry entry;
  entry.timestamp = GetCurrentTimestamp();
  entry.level = level;
  entry.message = buffer;

  // Set color based on log level
  if (strcmp(level, "ERROR") == 0)
  {
    entry.color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // Red
  }
  else if (strcmp(level, "WARN") == 0)
  {
    entry.color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); // Yellow
  }
  else if (strcmp(level, "INFO") == 0)
  {
    entry.color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); // Green
  }
  else
  {
    entry.color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Gray
  }

  g_LogBuffer.push_back(entry);

  // Write to file immediately if auto-save is enabled
  WriteLogToFile(entry.timestamp, entry.level, entry.message);

  // Keep buffer size under limit
  if (g_LogBuffer.size() > MAX_LOG_ENTRIES)
  {
    g_LogBuffer.erase(g_LogBuffer.begin());
  }
}

// Function to render logs window
void RenderLogsWindow()
{
  if (!g_LogsVisible)
    return;

  ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Debug Logs", &g_LogsVisible))
  {
    // Controls
    if (ImGui::Button("Clear"))
    {
      g_LogBuffer.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save to File"))
    {
      SaveLogsToFile();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &g_AutoScroll);
    ImGui::SameLine();
    ImGui::Checkbox("Auto-save", &g_AutoSaveToFile);
    ImGui::SameLine();
    ImGui::Text("(%d entries)", (int)g_LogBuffer.size());

    ImGui::Separator();

    // File path display
    ImGui::Text("Log file: %s", g_LogFilePath.c_str());
    if (ImGui::Button("Open Log Directory"))
    {
      // This will show path info
      LOGI("Log file location: %s", g_LogFilePath.c_str());
    }
    ImGui::Separator();

    // Log content
    if (ImGui::BeginChild("LogContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
      for (const auto &entry : g_LogBuffer)
      {
        ImGui::TextColored(entry.color, "[%s] [%s] %s",
                           entry.timestamp.c_str(), entry.level.c_str(), entry.message.c_str());
      }

      // Auto-scroll to bottom
      if (g_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
      {
        ImGui::SetScrollHereY(1.0f);
      }
    }
    ImGui::EndChild();
  }
  ImGui::End();
}

void hack();

class MyModule : public zygisk::ModuleBase
{
public:
  void onLoad(Api *api, JNIEnv *env) override
  {
    this->api_ = api;
    this->env_ = env;
    genv = env;
  }
  void preAppSpecialize(AppSpecializeArgs *args) override
  {
    static constexpr const char *packages[] = {
        "com.dts.freefiremax", "com.dts.freefireth"}; // Game Package Names
    const char *process = env_->GetStringUTFChars(args->nice_name, nullptr);
    for (const auto *package : packages)
    {
      is_game_ = (strcmp(process, package) == 0);
      if (is_game_)
      {
        break;
      }
    }
    env_->ReleaseStringUTFChars(args->nice_name, process);
  }
  void postAppSpecialize(const AppSpecializeArgs *args) override
  {
    if (is_game_)
    {
      genv->GetJavaVM(&jvm);
      std::thread{hack}.detach();
    }
  }

private:
  Api *api_ = nullptr;
  JNIEnv *env_ = nullptr;
  bool is_game_ = false;
};

void SetDarkGrayTheme()
{
  ImGuiStyle *style = &ImGui::GetStyle();
  ImVec4 *colors = style->Colors;

  colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
  colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
  colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
  colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
  colors[ImGuiCol_TabActive] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.220f, 0.220f, 0.220f, 1.000f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

  style->ChildRounding = 4.0f;
  style->FrameBorderSize = 1.0f;
  style->FrameRounding = 2.0f;
  style->GrabMinSize = 10.0f;
  style->PopupRounding = 2.0f;
  style->ScrollbarRounding = 12.0f;
  style->ScrollbarSize = 20.0f;
  style->TabBorderSize = 1.0f;
  style->TabRounding = 4.0f;
  style->WindowRounding = 4.0f;

  // Enhanced tab styling for fixed appearance
  style->TabMinWidthForCloseButton = 0.0f;
  style->WindowPadding = ImVec2(8.0f, 8.0f);
  style->ItemSpacing = ImVec2(8.0f, 4.0f);
}

//========BYPASS========\\

// Note: Bypass functions are placeholders for future anti-cheat bypass
// Currently unused - can be removed or implemented as needed

// void (*Bypass1)(void *_this);
// void _Bypass1(void *_this)
// {
//   LOGD("Bypass1 called - placeholder function");
//   return;
// }

// void (*Bypass2)(void *_this);
// void _Bypass2(void *_this)
// {
//   LOGD("Bypass2 called - placeholder function");
//   return;
// }

// ========================= \\

uintptr_t il2cpp_base = 0;
void *getRealAddr(ulong offset)
{
  return reinterpret_cast<void *>(il2cpp_base + offset);
};

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
  eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
  eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

  if (!g_IsSetup)
  {
    LOGD("Setting up ImGui...");
    prevWidth = g_GlWidth;
    prevHeight = g_GlHeight;
    SetupImgui();
    LOGI("ImGui setup completed - Screen: %dx%d", g_GlWidth, g_GlHeight);
    g_IsSetup = true;
  }

  ImGuiIO &io = ImGui::GetIO();
  SetDarkGrayTheme();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
  ImGui::NewFrame();
  int touchCount = (((int (*)())(Class_Input__get_touchCount))());
  if (touchCount > 0)
  {
    UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int))(Class_Input__GetTouch))(0);
    float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
    switch (touch.m_Phase)
    {
    case TouchPhase::Began:
    case TouchPhase::Stationary:
      io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
      io.MouseDown[0] = true;
      break;
    case TouchPhase::Ended:
    case TouchPhase::Canceled:
      io.MouseDown[0] = false;
      break;
    case TouchPhase::Moved:
      io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
      break;
    default:
      break;
    }
  }

  DrawESP(g_GlWidth, g_GlHeight);
  RenderLogsWindow();
  ImGui::SetNextWindowSize(ImVec2((float)g_GlWidth * 0.35f, (float)g_GlHeight * 0.60f), ImGuiCond_Once);
  if (ImGui::Begin(OBFUSCATE("Zygisk by Ngoc [ x32/x64 ]"), 0, ImGuiWindowFlags_NoBringToFrontOnFocus))
  {
    // Fixed Tab Bar at top
    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_FittingPolicyScroll))
    {
      // ESP Tab
      if (ImGui::BeginTabItem(OBFUSCATE("ESP")))
      {
        ImGui::Spacing();
        ImGui::Text("ESP Features");
        ImGui::Separator();

        Toggle(OBFUSCATE("Enable ESP"), &ESP_Enable);
        Toggle(OBFUSCATE("Box ESP"), &ESP_Box);
        Toggle(OBFUSCATE("Line ESP"), &ESP_Line);
        Toggle(OBFUSCATE("Health ESP"), &ESP_Health);
        Toggle(OBFUSCATE("Name ESP"), &ESP_Name);
        Toggle(OBFUSCATE("Distance ESP"), &ESP_Distance);
        Toggle(OBFUSCATE("FOV Circle"), &ESP_FOVCircle);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Spacing();
        if (ImGui::Button(OBFUSCATE("Enable All ESP"), ImVec2(-1, 0)))
        {
          ESP_Enable = true;
          ESP_Box = true;
          ESP_Line = true;
          ESP_Health = true;
          ESP_Name = true;
          ESP_Distance = true;
          ESP_FOVCircle = true;
        }
        if (ImGui::Button(OBFUSCATE("Disable All ESP"), ImVec2(-1, 0)))
        {
          ESP_Enable = false;
          ESP_Box = false;
          ESP_Line = false;
          ESP_Health = false;
          ESP_Name = false;
          ESP_Distance = false;
          ESP_FOVCircle = false;
        }

        ImGui::EndTabItem();
      }

      // Aimbot Tab
      if (ImGui::BeginTabItem(OBFUSCATE("Aimbot")))
      {
        ImGui::Spacing();
        Toggle("Enable AimBot Head", &Aimbot);
        ImGui::Separator();

        if (Aimbot)
        {
          ImGui::Text("Aim Settings:");
          ImGui::Combo("##AimDir", &AimWhen, dir, IM_ARRAYSIZE(dir));

          if (AimWhen == 0)
          {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Mode: Auto Aim");
          }
          else if (AimWhen == 1)
          {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Mode: Hold to Fire");
          }
          else if (AimWhen == 2)
          {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Mode: Hold to Scope");
          }

          ImGui::SliderFloat(OBFUSCATE("FOV Range"), &Fov_Aim, 0.0f, 500.0f, "%.0f");
          ImGui::SliderFloat(OBFUSCATE("Max Distance"), &Aimdis, 0.0f, 1000.0f, "%.0f");
        }
        else
        {
          ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Enable Aimbot to configure settings");
        }
        ImGui::EndTabItem();
      }

      // Settings Tab
      if (ImGui::BeginTabItem(OBFUSCATE("Settings")))
      {
        // General Settings
        ImGui::Text("General Settings");
        ImGui::Separator();

        if (ImGui::Checkbox(OBFUSCATE("Anti-Report"), &AntiReport))
        {
          if (AntiReport)
          {
            SetupAntiReport();
          }
        }
        ImGui::SameLine();
        if (ImGui::Button(OBFUSCATE("?")))
        {
          ImGui::OpenPopup(OBFUSCATE("Anti-Report Help"));
        }

        if (ImGui::BeginPopup(OBFUSCATE("Anti-Report Help")))
        {
          ImGui::Text("Anti-Report blocks other players from reporting you.");
          ImGui::Text("This helps protect your account from reports.");
          ImGui::Separator();
          ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Use responsibly!");
          ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Text("Version: 1.0.0 | Build: x32/x64");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Injected");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Game: Free Fire");

        ImGui::Spacing();
        if (ImGui::Button(OBFUSCATE("Reset All Settings"), ImVec2(-1, 0)))
        {
          ESP_Enable = false;
          ESP_Box = false;
          ESP_Line = false;
          ESP_Health = false;
          ESP_Name = false;
          ESP_Distance = false;
          ESP_FOVCircle = false;
          Aimbot = false;
          Fov_Aim = 50.0f;
          Aimdis = 200.0f;
          AntiReport = false;
        }
        ImGui::EndTabItem();
      }

      // Info Tab
      if (ImGui::BeginTabItem(OBFUSCATE("Info")))
      {
        ImGui::Spacing();
        ImGui::Text("About");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Zygisk Module by Ngoc");
        ImGui::Text("ESP & Aimbot for Free Fire");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Use responsibly!");

        ImGui::Spacing();
        ImGui::Separator();

        // Debug Controls
        ImGui::Text("Debug Options");
        ImGui::Separator();

        if (ImGui::Button("Show Debug Logs", ImVec2(-1, 0)))
        {
          g_LogsVisible = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Export All Logs", ImVec2(-1, 0)))
        {
          SaveLogsToFile();
          LOGI("Log export completed. Check file: %s", g_LogFilePath.c_str());
        }

        if (ImGui::Button("Test File Write", ImVec2(-1, 0)))
        {
          LOGD("Testing file write capability...");
          std::ofstream testFile(g_LogFilePath, std::ios::out | std::ios::app);
          if (testFile.is_open())
          {
            testFile << "=== TEST WRITE " << GetCurrentTimestamp() << " ===" << std::endl;
            testFile.close();
            LOGI("File write test successful");
          }
          else
          {
            LOGE("File write test failed - check permissions");
          }
        }

        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }
  }

  ImGui::End();
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  return old_eglSwapBuffers(dpy, surface);
}

inline void StartGUI()
{
  LOGD("Starting GUI initialization...");
  void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
  if (ptr_eglSwapBuffer != nullptr)
  {
    LOGI("eglSwapBuffers found at address: %p", ptr_eglSwapBuffer);
    DobbyHook((void *)ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
    LOGI("GUI started successfully");
  }
  else
  {
    LOGE("Failed to find eglSwapBuffers symbol");
  }
}

void hack_thread(pid_t pid)
{
  LOGD("=== ZYGISK FF HACK THREAD START ===");
  LOGD("Thread PID: %i", pid);

  for (int i = 0; i < 10; i++)
  {
    LOGD("Searching for libil2cpp.so... attempt %d/10", i + 1);
    il2cpp_base = get_module_base(pid, "libil2cpp.so");
    if (il2cpp_base != 0)
    {
      LOGI("libil2cpp.so found!");
      break;
    }
    LOGD("libil2cpp.so not found, waiting 10 seconds...");
    sleep(10);
  }

  if (il2cpp_base == 0)
  {
    LOGE("CRITICAL: libil2cpp.so not found after 10 attempts!");
    LOGE("Thread %d terminating", pid);
    std::terminate();
  }

  LOGI("IL2CPP base address: 0x%" PRIxPTR, il2cpp_base);
  LOGD("Waiting 10 seconds before attachment...");
  sleep(10);

  LOGD("Starting IL2CPP attachment...");
  Il2CppAttach();
  LOGI("IL2CPP attached successfully");
  // If you have Your own bypass then past here 🙂
  // =======

  //=======

  LOGD("Initializing GUI...");
  StartGUI();
  LOGD("=== ZYGISK FF INITIALIZATION COMPLETE ===");
}

void hack()
{
  // Initialize log file at start
  InitLogFile();

  LOGI("*** ZYGISK FF INJECTION SUCCESSFUL ***");
  LOGD("Starting hack thread...");
  std::thread thread_hack(hack_thread, getpid());
  thread_hack.detach();
  LOGD("Hack thread detached successfully");
}

REGISTER_ZYGISK_MODULE(MyModule)
