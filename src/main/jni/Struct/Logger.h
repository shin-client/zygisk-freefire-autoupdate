#pragma once

#include <android/log.h>
#include <imgui/imgui.h>

#include <chrono>
#include <cstdarg>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// Log system structures
struct LogEntry {
  std::string timestamp;
  std::string level;
  std::string message;
  ImVec4      color;
};

// External declarations
extern std::vector<LogEntry> g_LogBuffer;
extern bool                  g_AutoScroll;
extern bool                  g_LogsVisible;
extern bool                  g_AutoSaveToFile;
extern std::string           g_LogFilePath;
extern const int             MAX_LOG_ENTRIES;

// Function declarations
std::string GetCurrentTimestamp();
void        AddLog(const char* level, const char* fmt, ...);
void        RenderLogsWindow();
void        SaveLogsToFile();
void        InitLogFile();
void        WriteLogToFile(const std::string& timestamp, const std::string& level, const std::string& message);

// Logging macros
#define TAG "ZygiskFF"
#define LOGD(...)                                             \
  do {                                                        \
    __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__); \
    AddLog("DEBUG", __VA_ARGS__);                             \
  } while (0)
#define LOGE(...)                                             \
  do {                                                        \
    __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__); \
    AddLog("ERROR", __VA_ARGS__);                             \
  } while (0)
#define LOGI(...)                                            \
  do {                                                       \
    __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__); \
    AddLog("INFO", __VA_ARGS__);                             \
  } while (0)
#define LOGW(...)                                            \
  do {                                                       \
    __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__); \
    AddLog("WARN", __VA_ARGS__);                             \
  } while (0)
