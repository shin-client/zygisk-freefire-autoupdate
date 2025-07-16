#pragma once

#include "../KittyMemory/KittyInclude.h"
#include "Bools.h"
#include "Dobby/dobby.h"
#include "Logger.h"
#include "Unity/MonoString.h"
#include "Unity/Quaternion.h"
#include "obfuscate.h"

extern ElfScanner g_il2cppELF;

// ============================================================================
// UTILITY CLASSES
// ============================================================================

class Vvector3 {
 public:
  float X;
  float Y;
  float Z;

  Vvector3() : X(0), Y(0), Z(0) {}

  Vvector3(float x1, float y1, float z1) : X(x1), Y(y1), Z(z1) {}

  Vvector3(const Vvector3 &v);
  ~Vvector3();
};

Vvector3::Vvector3(const Vvector3 &v) : X(v.X), Y(v.Y), Z(v.Z) {}

Vvector3::~Vvector3() {}

// ============================================================================
// UNITY ENGINE OFFSETS
// ============================================================================

// Camera System
#define Class_Camera__get_main                                                                                  \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Camera"), \
                                   OBFUSCATE("get_main"))
#define Class_Camera__WorldToScreenPoint                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Camera"), \
                                   OBFUSCATE("WorldToScreenPoint"), 1)
#define Camera_get_fieldOfView \
  (uintptr_t)Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_fieldOfView")
#define Camera_set_fieldOfView \
  (uintptr_t)Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "set_fieldOfView", 1)

// Input System
#define Class_Input__get_touchCount                                                                            \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Input"), \
                                   OBFUSCATE("get_touchCount"))
#define Class_Input__GetTouch                                                                                  \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Input"), \
                                   OBFUSCATE("GetTouch"), 1)
#define Class_Input__get_mousePosition                                                                         \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Input"), \
                                   OBFUSCATE("get_mousePosition"))

// Screen System
#define Class_Screen__get_width                                                                                 \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Screen"), \
                                   OBFUSCATE("get_width"))
#define Class_Screen__get_height                                                                                \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Screen"), \
                                   OBFUSCATE("get_height"))
#define Class_Screen__get_density                                                                               \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Screen"), \
                                   OBFUSCATE("get_dpi"))

// Transform System
#define Class_Transform__GetPosition                                                                    \
  Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                        OBFUSCATE("get_position_Injected"), 1)
#define Class_Transform__SetPosition                                                                    \
  Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                        OBFUSCATE("set_position_Injected"), 1)
#define Class_Transform__get_position                                                                              \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                                   OBFUSCATE("get_position"), 0)
#define Class_Transform__Position                                                                       \
  Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                        OBFUSCATE("get_position"), 0)
#define Class_Transform__Rotation                                                                       \
  Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                        OBFUSCATE("get_rotation"), 0)
#define ForWard                                                                                                    \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"), \
                                   OBFUSCATE("get_forward"), 0)

// Component System
#define Class_Component__get_transform                                                                             \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Component"), \
                                   OBFUSCATE("get_transform"), 0)
#define Class_Compent__Transform                                                                        \
  Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Component"), \
                        OBFUSCATE("get_transform"), 0)

// Time System
#define TimeSys                                                                                               \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Time"), \
                                   OBFUSCATE("get_deltaTime"), 0)

// ============================================================================
// GAME SPECIFIC OFFSETS
// ============================================================================

// Player System
#define ListPlayer                                                                             \
  (uintptr_t)Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), \
                                  OBFUSCATE("NFJPHMKKEBF"), OBFUSCATE("HOOCHDLKOOG"))
#define EnemyUpdate                                                                                                  \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("LateUpdate"), 0)
#define MainCam                                                                                                     \
  (uintptr_t)Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                  OBFUSCATE("MainCameraTransform"))
#define Head                                                                                                         \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("GetHeadTF"), 0)
#define HeadColider                                                                                                  \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_HeadCollider"), 0)

// Player Status
#define Visible                                                                                                      \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("IsVisible"), 0)
#define Team                                                                                                         \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("IsLocalTeammate"), 0)
#define Die                                                                                                          \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_IsDieing"), 0)
#define LocalBool                                                                                                    \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("IsLocalPlayer"), 0)

// Player Health
#define CurHP                                                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_CurHP"), 0)
#define MaxHP                                                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_MaxHP"), 0)

// Player Info
#define PlayerName                                                                                                   \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_NickName"), 0)

// Player Actions
#define Aim                                                                                                          \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("SetAimRotation"), 1)
#define Scope                                                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_IsSighting"), 0)
#define Fire                                                                                                         \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("IsFiring"), 0)

// Vehicle System
#define CarLocal                                                                                                     \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("VehicleIAmIn"), 0)
#define GetCar                                                                                                       \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), \
                                   OBFUSCATE("get_IsDriver"), 0)

// Game Management
#define Match                                                                                                   \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("GameFacade"), \
                                   OBFUSCATE("CurrentMatch"), 0)
#define Local                                                                          \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), \
                                   OBFUSCATE("UIHudDetectorController"), OBFUSCATE("GetLocalPlayer"), 0)

// Physics System
#define Raycast                                                                                 \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), \
                                   OBFUSCATE("JEAGCMACNNC"), OBFUSCATE("PLDCHDBCOBF"), 4)

// String System
#define CharGet                                                                                         \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("mscorlib.dll"), OBFUSCATE("System"), OBFUSCATE("String"), \
                                   OBFUSCATE("get_Chars"), 1)

// Reset Guest
#define ResetGuest_Offset                                                                                       \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("GameConfig"), \
                                   OBFUSCATE("get_ResetGuest"), 0)

// ============================================================================
// BYPASS
// ============================================================================

#define Report1                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), \
                                   OBFUSCATE("InteractionPopUtil"), OBFUSCATE("CreateReportPlayer"), 3)
#define Report2                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), \
                                   OBFUSCATE("UIHudMatchResultItemBaseController"), OBFUSCATE("SetReportData"), 4)
#define Report3                                                                                                   \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("UIModelVoice"), \
                                   OBFUSCATE("get_ReportedPlayerTimeDict"), 0)
#define Report4                                                                        \
  (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), \
                                   OBFUSCATE("InteractionPopUtil"), OBFUSCATE("CreateAddToBlackList"), 3)

// ============================================================================
// BYPASS HOOK FUNCTIONS
// ============================================================================

static void *(*orig_CreateReportPlayer)(void *util, void *player, int reason) = nullptr;

static void *hook_CreateReportPlayer(void *util, void *player, int reason) {
  if (g_OtherConfig->AntiReport) {
    // Block report - do nothing and return null
    return nullptr;
  }
  // Call original function if anti-report is disabled
  return orig_CreateReportPlayer(util, player, reason);
}

static void *(*orig_SetReportData)(void *controller, void *data1, void *data2, void *data3, void *data4) = nullptr;

static void *hook_SetReportData(void *controller, void *data1, void *data2, void *data3, void *data4) {
  if (g_OtherConfig->AntiReport) {
    // Block report data setting
    return nullptr;
  }
  return orig_SetReportData(controller, data1, data2, data3, data4);
}

static void *(*orig_CreateAddToBlackList)(void *util, void *player, int reason) = nullptr;

static void *hook_CreateAddToBlackList(void *util, void *player, int reason) {
  if (g_OtherConfig->AntiReport) {
    return nullptr;
  }
  return orig_CreateAddToBlackList(util, player, reason);
}

// ============================================================================
// UNITY ENGINE FUNCTIONS
// ============================================================================

// Camera Functions
void *get_main() { return reinterpret_cast<void *(__fastcall *)()>(Class_Camera__get_main)(); }

static void *Camera_main() {
  void *(*_Camera_main)(void *nuls) = (void *(*)(void *))(Class_Camera__get_main);
  return _Camera_main(nullptr);
}

float get_fieldOfView() { return reinterpret_cast<float(__fastcall *)(void *)>(Camera_get_fieldOfView)(get_main()); }

void *set_fieldOfView(float value) {
  return reinterpret_cast<void *(__fastcall *)(void *, float)>(Camera_set_fieldOfView)(get_main(), value);
}

static Vector3 WorldToScreenPoint(void *WorldCam, Vector3 WorldPos) {
  Vector3 (*_WorldToScreenScene)(void *Camera, Vector3 position) =
      (Vector3 (*)(void *, Vector3))(Class_Camera__WorldToScreenPoint);
  return _WorldToScreenScene(WorldCam, WorldPos);
}

// Transform Functions
static void Transform_INTERNAL_SetPosition(void *player, Vvector3 inn) {
  void (*Transform_INTERNAL_SetPosition)(void *transform, Vvector3 in) =
      (void (*)(void *, Vvector3))(Class_Transform__SetPosition);
  Transform_INTERNAL_SetPosition(player, inn);
}

static Quaternion GetRotation(void *player) {
  Quaternion (*_GetRotation)(void *players) = (Quaternion (*)(void *))(Class_Transform__Rotation);
  return _GetRotation(player);
}

static Vector3 Transform_GetPosition(void *player) {
  Vector3 out = Vector3::zero();
  void (*_Transform_GetPosition)(void *transform, Vector3 *out) =
      (void (*)(void *, Vector3 *))(Class_Transform__GetPosition);
  _Transform_GetPosition(player, &out);
  return out;
}

Vector3 get_position(void *player) {
  Vector3 (*_get_position)(void *players) = (Vector3 (*)(void *))(Class_Transform__Position);
  return _get_position(player);
}

static void *Component_GetTransform(void *player) {
  void *(*_Component_GetTransform)(void *component) = (void *(*)(void *))(Class_Compent__Transform);
  return _Component_GetTransform(player);
}

static Vector3 GetForward(void *player) {
  Vector3 (*_GetForward)(void *players) = (Vector3 (*)(void *))(ForWard);
  return _GetForward(player);
}

Vector3 getPosition(void *transform) { return get_position(Component_GetTransform(transform)); }

// Screen Functions
int get_width() { return reinterpret_cast<int(__fastcall *)()>(Class_Screen__get_width)(); }

int get_height() { return reinterpret_cast<int(__fastcall *)()>(Class_Screen__get_height)(); }

float get_density() { return reinterpret_cast<float(__fastcall *)()>(Class_Screen__get_density)(); }

// Physics Functions
static bool Physics_Raycast(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider) {
  bool (*_Physics_Raycast)(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider) =
      (bool (*)(Vector3, Vector3, unsigned int, void *))(Raycast);
  return _Physics_Raycast(camLocation, headLocation, LayerID, collider);
}

// ============================================================================
// GAME SPECIFIC FUNCTIONS
// ============================================================================

// Game Management
static void *Curent_Match() {
  void *(*_Curent_Match)(void *nuls) = (void *(*)(void *))(Match);
  return _Curent_Match(NULL);
}

static void *GetLocalPlayer(void *Game) {
  void *(*_GetLocalPlayer)(void *match) = (void *(*)(void *))(Local);
  return _GetLocalPlayer(Game);
}

static void Patch_ResetGuest(bool enable) {
  uintptr_t il2cppBase = g_il2cppELF.base();

  LOGD("ResetGuest_Offset: %d", ResetGuest_Offset);

  MemoryPatch patch =
      MemoryPatch::createWithBytes(il2cppBase + ResetGuest_Offset, "\x01\x00\xA0\xE3\x1E\xFF\x2F\xE1", 8);

  if (enable) {
    if (!patch.Modify()) {
      LOGE("Patch ResetGuest failed!");
    }
  } else {
    if (!patch.Restore()) {
      LOGE("Restore ResetGuest failed!");
    }
  }
}

// Player Status Functions
static bool get_isVisible(void *player) {
  bool (*_get_isVisible)(void *players) = (bool (*)(void *))(Visible);
  return _get_isVisible(player);
}

static bool get_isLocalTeam(void *player) {
  bool (*_get_isLocalTeam)(void *players) = (bool (*)(void *))(Team);
  return _get_isLocalTeam(player);
}

static bool get_IsDieing(void *player) {
  bool (*_get_die)(void *players) = (bool (*)(void *))(Die);
  return _get_die(player);
}

static bool IsLocal(void *player) {
  bool (*_isMe)(void *players) = (bool (*)(void *))(LocalBool);
  return _isMe(player);
}

static bool get_IsSighting(void *player) {
  bool (*_get_IsSighting)(void *players) = (bool (*)(void *))(Scope);
  return _get_IsSighting(player);
}

static bool get_IsFiring(void *player) {
  bool (*_get_IsFiring)(void *players) = (bool (*)(void *))(Fire);
  return _get_IsFiring(player);
}

// Player Health Functions
static int GetHp(void *player) {
  int (*_GetHp)(void *players) = (int (*)(void *))(CurHP);
  return _GetHp(player);
}

static int get_MaxHP(void *enemy) {
  int (*_get_MaxHP)(void *player) = (int (*)(void *))(MaxHP);
  return _get_MaxHP(enemy);
}

// Player Position Functions
static void *GetHeadPositions(void *player) {
  void *(*_GetHeadPositions)(void *players) = (void *(*)(void *))(Head);
  return _GetHeadPositions(player);
}

static void *Player_GetHeadCollider(void *player) {
  void *(*_Player_GetHeadCollider)(void *players) = (void *(*)(void *))(HeadColider);
  return _Player_GetHeadCollider(player);
}

static Vector3 GetHeadPosition(void *player) { return get_position(GetHeadPositions(player)); }

static Vector3 CameraMain(void *player) { return get_position(*(void **)((uint64_t)player + MainCam)); }

// Player Action Functions
static void set_aim(void *player, Quaternion look) {
  void (*_set_aim)(void *players, Quaternion lock) = (void (*)(void *, Quaternion))(Aim);
  _set_aim(player, look);
}

// Vehicle Functions
static void *GetLocalCar(void *playerCar) {
  void *(*_Player_get_local)(void *Player) = (void *(*)(void *))(CarLocal);
  return _Player_get_local(playerCar);
}

static bool IsDriver(void *player) {
  bool (*_IsDriver)(void *players) = (bool (*)(void *))(GetCar);
  return _IsDriver(player);
}

// Player Info Functions
static MonoString *get_NickName(void *player) {
  MonoString *(*_get_NickName)(void *players) = (MonoString * (*)(void *))(PlayerName);
  return _get_NickName(player);
}

// ============================================================================
// STRING PROCESSING FUNCTIONS
// ============================================================================

char get_Chars(MonoString *str, int index) {
  char (*_get_Chars)(MonoString *str, int index) = (char (*)(MonoString *, int))(CharGet);
  return _get_Chars(str, index);
}

// Better UTF-8 support function
wchar_t get_Chars_Wide(MonoString *str, int index) {
  wchar_t (*_get_Chars)(MonoString *str, int index) = (wchar_t (*)(MonoString *, int))(CharGet);
  return _get_Chars(str, index);
}

// Safe UTF-8 string extraction with full Unicode support and error handling
std::string get_UTF8_String_Safe(MonoString *str) {
  if (str == nullptr) return "";

  int len = str->getLength();
  if (len <= 0 || len > 1000) return "";  // Safety check

  try {
    std::string utf8_str;
    utf8_str.reserve(len * 3);  // Reserve space for efficiency

    for (int i = 0; i < len; i++) {
      wchar_t wch = get_Chars_Wide(str, i);

      // Skip null characters
      if (wch == 0) continue;

      // Convert to UTF-8 with full Unicode support
      if (wch < 0x80) {
        utf8_str += (char)wch;
      } else if (wch < 0x800) {
        utf8_str += (char)(0xC0 | (wch >> 6));
        utf8_str += (char)(0x80 | (wch & 0x3F));
      } else if (wch < 0xD800 || wch > 0xDFFF) {
        utf8_str += (char)(0xE0 | (wch >> 12));
        utf8_str += (char)(0x80 | ((wch >> 6) & 0x3F));
        utf8_str += (char)(0x80 | (wch & 0x3F));
      } else if (wch >= 0xD800 && wch <= 0xDBFF && i + 1 < len) {
        // Handle surrogate pairs for 4-byte UTF-8 (emojis, rare characters)
        wchar_t low = get_Chars_Wide(str, i + 1);
        if (low >= 0xDC00 && low <= 0xDFFF)  // Valid low surrogate
        {
          // Calculate the full Unicode codepoint
          uint32_t codepoint = 0x10000 + ((wch & 0x3FF) << 10) + (low & 0x3FF);

          // Encode as 4-byte UTF-8
          utf8_str += (char)(0xF0 | (codepoint >> 18));
          utf8_str += (char)(0x80 | ((codepoint >> 12) & 0x3F));
          utf8_str += (char)(0x80 | ((codepoint >> 6) & 0x3F));
          utf8_str += (char)(0x80 | (codepoint & 0x3F));

          i++;
        } else {
          utf8_str += "\xEF\xBF\xBD";
        }
      } else {
        utf8_str += "\xEF\xBF\xBD";
      }
    }

    if (utf8_str.length() > len * 4) {
      throw std::runtime_error("UTF-8 result too long");
    }

    return utf8_str;
  } catch (...) {
    std::string ascii_str;
    for (int i = 0; i < len && i < 100; i++) {
      char ch = get_Chars(str, i);
      if (ch >= 32 && ch <= 126) {
        ascii_str += ch;
      } else if (ch == 0) {
        break;
      }
    }
    return ascii_str.empty() ? "***" : ascii_str;
  }
}

// Alias for backward compatibility
#define get_UTF8_String get_UTF8_String_Safe

// ============================================================================
// ANTI-REPORT SYSTEM SETUP
// ============================================================================

// Anti-Report Setup Function
void SetupAntiReport() {
  LOGI("Setting up Anti-Report system...");

  // Hook CreateReportPlayer
  if (Report1) {
    LOGD("Hooking CreateReportPlayer at: %p", (void *)Report1);
    orig_CreateReportPlayer = (void *(*)(void *, void *, int))Report1;
    DobbyHook((void *)Report1, (void *)hook_CreateReportPlayer, (void **)&orig_CreateReportPlayer);
    LOGI("CreateReportPlayer hook installed successfully");
  } else {
    LOGE("Failed to find CreateReportPlayer address");
  }

  // Hook SetReportData
  if (Report2) {
    LOGD("Hooking SetReportData at: %p", (void *)Report2);
    orig_SetReportData = (void *(*)(void *, void *, void *, void *, void *))Report2;
    DobbyHook((void *)Report2, (void *)hook_SetReportData, (void **)&orig_SetReportData);
    LOGI("SetReportData hook installed successfully");
  } else {
    LOGE("Failed to find SetReportData address");
  }

  // Hook CreateAddToBlackList
  if (Report4) {
    LOGD("Hooking CreateAddToBlackList at: %p", (void *)Report4);
    orig_CreateAddToBlackList = (void *(*)(void *, void *, int))Report4;
    DobbyHook((void *)Report4, (void *)hook_CreateAddToBlackList, (void **)&orig_CreateAddToBlackList);
    LOGI("CreateAddToBlackList hook installed successfully");
  } else {
    LOGE("Failed to find CreateAddToBlackList address");
  }

  LOGI("Anti-Report system setup completed");
}

// ============================================================================
// AUTO UPDATE OFFSET - NO NEED TO CHANGE ANYTHING
// ============================================================================
