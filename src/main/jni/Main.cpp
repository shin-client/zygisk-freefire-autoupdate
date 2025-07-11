#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <limits>

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

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

// Global variables for screen dimensions
int g_GlWidth, g_GlHeight;

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

//========BYPASS========\\

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
  colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
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
  style->GrabMinSize = 7.0f;
  style->PopupRounding = 2.0f;
  style->ScrollbarRounding = 12.0f;
  style->ScrollbarSize = 13.0f;
  style->TabBorderSize = 1.0f;
  style->TabRounding = 0.0f;
  style->WindowRounding = 4.0f;
}

void (*Bypass1)(void *_this);
void _Bypass1(void *_this)
{
  return;
}

void (*Bypass2)(void *_this);
void _Bypass2(void *_this)
{
  return;
}

// ========================= \\

uintptr_t il2cpp_base = 0;
void *getRealAddr(ulong offset)
{
  return reinterpret_cast<void *>(il2cpp_base + offset);
};

namespace Settings
{
  static int Tab = 1;
}

bool clearMousePos = true;
bool initImGui = false;

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
  eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
  eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

  if (!g_IsSetup)
  {
    prevWidth = g_GlWidth;
    prevHeight = g_GlHeight;
    SetupImgui();

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
      clearMousePos = true;
      break;
    case TouchPhase::Moved:
      io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
      break;
    default:
      break;
    }
  }

  DrawESP(g_GlWidth, g_GlHeight);
  ImGui::SetNextWindowSize(ImVec2((float)g_GlWidth * 0.35f, (float)g_GlHeight * 0.60f), ImGuiCond_Once); // Tăng kích thước để có không gian cho tabs
  // Zygisk Menu // You can Change here
  if (ImGui::Begin(OBFUSCATE("Zygisk by Ngoc [ x32/x64 ]"), 0, ImGuiWindowFlags_NoBringToFrontOnFocus))
  {

    // Tab Bar
    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None))
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
          ESP_Enable = true;
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
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Mode: Hold to Aim");
          }
          else if (AimWhen == 2)
          {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Mode: Toggle Aim");
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
        ImGui::Spacing();
        ImGui::Text("General Settings");
        ImGui::Separator();

        // Anti-Report Settings
        ImGui::Spacing();
        ImGui::Text("Protection Features");
        ImGui::Separator();

        if (ImGui::Checkbox(OBFUSCATE("Anti-Report"), &AntiReport))
        {
          if (AntiReport)
          {
            // Enable anti-report when toggled on
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
        ImGui::Separator();

        // Có thể thêm các settings khác ở đây
        ImGui::Text("Version: 1.0.0");
        ImGui::Text("Build: x32/x64");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Injected");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Game: Free Fire");

        ImGui::Spacing();
        if (ImGui::Button(OBFUSCATE("Reset All Settings"), ImVec2(-1, 0)))
        {
          // Reset logic
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
        ImGui::Text("Advanced ESP & Aimbot for Free Fire");
        ImGui::Separator();

        ImGui::Text("Features:");
        ImGui::BulletText("ESP System with multiple options");
        ImGui::BulletText("Advanced Aimbot with FOV control");
        ImGui::BulletText("Zygisk injection for stability");
        ImGui::BulletText("x32/x64 architecture support");

        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Use responsibly!");
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

typedef unsigned long DWORD;
static uintptr_t libBase;

inline void StartGUI()
{
  void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
  if (ptr_eglSwapBuffer != nullptr)
  {
    DobbyHook((void *)ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
    LOGD("GUI started successfully");
  }
}

void hack_thread(pid_t pid)
{
  LOGD("Hack Thread initiated for PID: %i", pid);

  for (int i = 0; i < 10; i++)
  {
    il2cpp_base = get_module_base(pid, "libil2cpp.so");
    if (il2cpp_base != 0)
      break;
    sleep(10);
  }

  if (il2cpp_base == 0)
  {
    LOGE("libil2cpp.so not found in thread %d", pid);
    std::terminate();
  }

  LOGD("IL2CPP base address: 0x%" PRIxPTR, il2cpp_base);
  sleep(10);

  Il2CppAttach();
  // If you have Your own bypass then past here 🙂
  // =======

  //=======

  StartGUI();
}

void hack()
{
  LOGD("Inject Ok");
  std::thread thread_hack(hack_thread, getpid());
  thread_hack.detach();
}

REGISTER_ZYGISK_MODULE(MyModule)
