#include "Struct/main.h"

#include <Il2Cpp.h>
#include <KittyMemory.h>
#include <KittyUtils.h>
#include <android/log.h>
#include <unistd.h>
#include <xdl/xdl.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "Struct/Class.h"
#include "Struct/Gui.hpp"
#include "Struct/main.h"
#include "Struct/obfuscate.h"
#include "imgui/Toggle.h"
#include "imgui/fonts/FontAwesome6_solid.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

ElfScanner g_il2cppELF;

void hack();

class MyModule : public zygisk::ModuleBase {
 public:
  void onLoad(Api *api, JNIEnv *env) override {
    this->api_ = api;
    this->env_ = env;
    genv       = env;
  }

  void preAppSpecialize(AppSpecializeArgs *args) override {
    static constexpr const char *packages[] = {"com.dts.freefiremax", "com.dts.freefireth"};  // Game Package Names
    const char                  *process    = env_->GetStringUTFChars(args->nice_name, nullptr);
    for (const auto *package : packages) {
      is_game_ = (strcmp(process, package) == 0);
      if (is_game_) {
        break;
      }
    }
    env_->ReleaseStringUTFChars(args->nice_name, process);
  }

  void postAppSpecialize(const AppSpecializeArgs *args) override {
    if (is_game_) {
      genv->GetJavaVM(&jvm);
      std::thread{hack}.detach();
    }
  }

 private:
  Api    *api_     = nullptr;
  JNIEnv *env_     = nullptr;
  bool    is_game_ = false;
};

void SetDarkGrayTheme() {
  ImGuiStyle *style  = &ImGui::GetStyle();
  ImVec4     *colors = style->Colors;

  colors[ImGuiCol_Text]                  = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
  colors[ImGuiCol_TextDisabled]          = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
  colors[ImGuiCol_WindowBg]              = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
  colors[ImGuiCol_ChildBg]               = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
  colors[ImGuiCol_PopupBg]               = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
  colors[ImGuiCol_Border]                = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
  colors[ImGuiCol_BorderShadow]          = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
  colors[ImGuiCol_FrameBg]               = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
  colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
  colors[ImGuiCol_FrameBgActive]         = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
  colors[ImGuiCol_TitleBg]               = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_TitleBgActive]         = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
  colors[ImGuiCol_MenuBarBg]             = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
  colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
  colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
  colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
  colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_CheckMark]             = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
  colors[ImGuiCol_SliderGrab]            = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
  colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_Button]                = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
  colors[ImGuiCol_ButtonHovered]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
  colors[ImGuiCol_ButtonActive]          = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
  colors[ImGuiCol_Header]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
  colors[ImGuiCol_HeaderHovered]         = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_HeaderActive]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_Separator]             = colors[ImGuiCol_Border];
  colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
  colors[ImGuiCol_SeparatorActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_ResizeGrip]            = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
  colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
  colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_Tab]                   = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
  colors[ImGuiCol_TabHovered]            = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
  colors[ImGuiCol_TabActive]             = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
  colors[ImGuiCol_TabUnfocused]          = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
  colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.220f, 0.220f, 0.220f, 1.000f);
  colors[ImGuiCol_PlotLines]             = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
  colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_PlotHistogram]         = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
  colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_TextSelectedBg]        = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
  colors[ImGuiCol_DragDropTarget]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavHighlight]          = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
  colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
  colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

  style->ChildRounding     = 4.0f;
  style->FrameBorderSize   = 1.0f;
  style->FrameRounding     = 2.0f;
  style->GrabMinSize       = 10.0f;
  style->PopupRounding     = 2.0f;
  style->ScrollbarRounding = 12.0f;
  style->ScrollbarSize     = 20.0f;
  style->TabBorderSize     = 1.0f;
  style->TabRounding       = 4.0f;
  style->WindowRounding    = 4.0f;

  // Enhanced tab styling for fixed appearance
  style->TabMinWidthForCloseButton = 0.0f;
  style->WindowPadding             = ImVec2(8.0f, 8.0f);
  style->ItemSpacing               = ImVec2(8.0f, 4.0f);
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

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
  eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
  eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

  if (!g_IsSetup) {
    LOGD("Setting up ImGui...");
    prevWidth  = g_GlWidth;
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
  if (touchCount > 0) {
    UnityEngine_Touch_Fields touch    = ((UnityEngine_Touch_Fields (*)(int))(Class_Input__GetTouch))(0);
    float                    reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
    switch (touch.m_Phase) {
      case TouchPhase::Began:
      case TouchPhase::Stationary:
        io.MousePos     = ImVec2(touch.m_Position.fields.x, reverseY);
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
  ImGui::SetNextWindowSize(ImVec2((float)g_GlWidth * 0.35f, (float)g_GlHeight * 0.50f), ImGuiCond_Once);
  if (ImGui::Begin(OBFUSCATE("Zygisk by Ngoc [ x32/x64 ]"), 0, ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    static int   currentMenu = 0;
    static float item_height = 50.0f;

    // Sidebar
    ImGui::BeginChild("Sidebar", ImVec2(150, 0), true);
    {
      if (ImGui::Selectable(OBFUSCATE("ESP"), currentMenu == 0, 0, ImVec2(0, item_height))) currentMenu = 0;
      if (ImGui::Selectable(OBFUSCATE("Aimbot"), currentMenu == 1, 0, ImVec2(0, item_height))) currentMenu = 1;
      if (ImGui::Selectable(OBFUSCATE("Settings"), currentMenu == 2, 0, ImVec2(0, item_height))) currentMenu = 2;
      if (ImGui::Selectable(OBFUSCATE("Info"), currentMenu == 3, 0, ImVec2(0, item_height))) currentMenu = 3;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Content
    ImGui::BeginChild("Content", ImVec2(0, 0), true);
    {
      switch (currentMenu) {
        case 0: {
          ImGui::Spacing();
          ImGui::Text("ESP Features");
          ImGui::Separator();

          Toggle(OBFUSCATE("Enable ESP"), &g_ESPConfig->ESP_Enable);
          Toggle(OBFUSCATE("Box ESP"), &g_ESPConfig->ESP_Box);
          Toggle(OBFUSCATE("Line ESP"), &g_ESPConfig->ESP_Line);
          Toggle(OBFUSCATE("Health ESP"), &g_ESPConfig->ESP_Health);
          Toggle(OBFUSCATE("Name ESP"), &g_ESPConfig->ESP_Name);
          Toggle(OBFUSCATE("Distance ESP"), &g_ESPConfig->ESP_Distance);

          ImGui::Spacing();
          ImGui::Separator();

          ImGui::Spacing();
          if (ImGui::Button(OBFUSCATE("Enable All ESP"), ImVec2(-1, 0))) {
            g_ESPConfig->enableAll();
          }
          if (ImGui::Button(OBFUSCATE("Disable All ESP"), ImVec2(-1, 0))) {
            g_ESPConfig->disableAll();
          }
          break;
        }
        case 1: {
          ImGui::Spacing();
          Toggle("Enable AimBot Head", &g_AimbotConfig->Aimbot);
          ImGui::Separator();

          if (g_AimbotConfig->Aimbot) {
            ImGui::Text("Aim Settings:");
            ImGui::Combo("##AimDir", &g_AimbotConfig->AimWhen, dir, DIR_COUNT);

            if (g_AimbotConfig->AimWhen == 0) {
              ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Mode: Auto Aim");
            } else if (g_AimbotConfig->AimWhen == 1) {
              ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Mode: Hold to Fire");
            } else if (g_AimbotConfig->AimWhen == 2) {
              ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Mode: Hold to Scope");
            }

            ImGui::SliderFloat(OBFUSCATE("FOV Range"), &g_AimbotConfig->Fov_Aim, 0.0f, 500.0f, "%.0f");
            ImGui::SliderFloat(OBFUSCATE("Max Distance"), &g_AimbotConfig->Aimdis, 0.0f, 1000.0f, "%.0f");
          } else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Enable Aimbot to configure settings");
          }
          break;
        }
        case 2: {
          ImGui::Text("General Settings");
          ImGui::Separator();

          if (ImGui::Checkbox(OBFUSCATE("Anti-Report"), &g_OtherConfig->AntiReport)) {
            if (g_OtherConfig->AntiReport) {
              SetupAntiReport();
            }
          }

          if (ImGui::Checkbox(OBFUSCATE("Reset Guest"), &g_OtherConfig->ResetGuest)) {
            Patch_ResetGuest(g_OtherConfig->ResetGuest);
          }

          ImGui::Spacing();
          ImGui::Text("Version: 1.0.0 | Build: x32/x64");
          ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Injected");
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Game: Free Fire");

          ImGui::Spacing();
          if (ImGui::Button(OBFUSCATE("Reset All Settings"), ImVec2(-1, 0))) {
            g_OtherConfig->resetAll();
          }
          break;
        }
        case 3: {
          ImGui::Spacing();
          ImGui::Text("About");
          ImGui::Separator();

          ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Zygisk Module by Ngoc");
          ImGui::Text("ESP & Aimbot for Free Fire");
          ImGui::Separator();

          ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Use responsibly!");

          ImGui::Spacing();
          ImGui::Separator();

          ImGui::Text("Debug Options");
          ImGui::Separator();

          if (ImGui::Button("Show Debug Logs", ImVec2(-1, 0))) {
            g_LogsVisible = true;
          }

          ImGui::Spacing();
          if (ImGui::Button("Export All Logs", ImVec2(-1, 0))) {
            SaveLogsToFile();
            LOGI("Log export completed. Check file: %s", g_LogFilePath.c_str());
          }
          break;
        }
      }
    }
    ImGui::EndChild();
  }

  ImGui::End();
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  return old_eglSwapBuffers(dpy, surface);
}

inline void StartGUI() {
  LOGD("Starting GUI initialization...");
  void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
  if (ptr_eglSwapBuffer != nullptr) {
    LOGI("eglSwapBuffers found at address: %p", ptr_eglSwapBuffer);
    DobbyHook((void *)ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
    LOGI("GUI started successfully");
  } else {
    LOGE("Failed to find eglSwapBuffers symbol");
  }
}

void hack_thread(pid_t pid) {
  LOGD("=== ZYGISK FF HACK THREAD START ===");
  LOGD("Thread PID: %i", pid);

  do {
    sleep(1);
    LOGD("Searching for libil2cpp.so...");
    g_il2cppELF = ElfScanner::createWithPath("libil2cpp.so");
  } while (!g_il2cppELF.isValid());

  uintptr_t il2cppBase = g_il2cppELF.base();

  if (il2cppBase == 0) {
    LOGE("CRITICAL: libil2cpp.so not found!");
    LOGE("Thread %d terminating", pid);
    std::terminate();
  }

  LOGI("il2cppBase: 0x%" PRIxPTR, il2cppBase);
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

void hack() {
  InitLogFile();

  LOGI("*** ZYGISK FF INJECTION SUCCESSFUL ***");
  LOGD("Starting hack thread...");
  std::thread thread_hack(hack_thread, getpid());
  thread_hack.detach();
  LOGD("Hack thread detached successfully");
}

REGISTER_ZYGISK_MODULE(MyModule)
