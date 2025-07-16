#include "Logger.h"

#include <vector>

std::vector<LogEntry> g_LogBuffer;
bool                  g_AutoScroll     = true;
bool                  g_LogsVisible    = false;
bool                  g_AutoSaveToFile = true;
std::string           g_LogFilePath    = "/sdcard/zygisk_ff_logs.txt";
const int             MAX_LOG_ENTRIES  = 1000;

std::string GetCurrentTimestamp() {
  auto now    = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  auto ms     = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
  ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
  return ss.str();
}

void InitLogFile() {
  if (!g_AutoSaveToFile) return;

  LOGD("Initializing log file: %s", g_LogFilePath.c_str());

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::trunc);
  if (!logFile.is_open()) {
    // Try alternative paths
    std::vector<std::string> altPaths = {"/storage/emulated/0/Download/zygisk_ff_logs.txt",
                                         "/data/data/com.dts.freefiremax/zygisk_ff_logs.txt",
                                         "/data/data/com.dts.freefireth/zygisk_ff_logs.txt"};

    for (const auto &altPath : altPaths) {
      LOGW("Trying alternative path: %s", altPath.c_str());
      logFile.open(altPath, std::ios::out | std::ios::trunc);
      if (logFile.is_open()) {
        g_LogFilePath = altPath;
        LOGI("Successfully switched to log path: %s", altPath.c_str());
        break;
      }
    }

    if (!logFile.is_open()) {
      LOGE("Failed to initialize any log file path");
      g_AutoSaveToFile = false;  // Disable auto-save if no path works
      return;
    }
  }

  try {
    logFile << "=== ZYGISK FF LOG SESSION START ===" << std::endl;
    logFile << "Session started at: " << GetCurrentTimestamp() << std::endl;
    logFile << "=====================================" << std::endl;
    logFile.flush();
    logFile.close();
    LOGI("Log file initialized successfully");
  } catch (...) {
    LOGE("Failed to write to log file during initialization");
    logFile.close();
    g_AutoSaveToFile = false;
  }
}

void WriteLogToFile(const std::string &timestamp, const std::string &level, const std::string &message) {
  if (!g_AutoSaveToFile) return;

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::app);
  if (!logFile.is_open()) {
    // Try alternative path if main path fails
    std::string altPath = "/data/data/com.dts.freefireth/zygisk_ff_logs.txt";
    logFile.open(altPath, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
      return;  // Silently fail to avoid spam
    }
    g_LogFilePath = altPath;  // Update global path
  }

  try {
    logFile << "[" << timestamp << "] [" << level << "] " << message << std::endl;
    logFile.flush();  // Ensure immediate write
    logFile.close();
  } catch (...) {
    // Silently handle write errors
    logFile.close();
  }
}

void SaveLogsToFile() {
  LOGD("Attempting to save logs to file: %s", g_LogFilePath.c_str());

  std::ofstream logFile(g_LogFilePath, std::ios::out | std::ios::trunc);
  if (!logFile.is_open()) {
    LOGE("Failed to open log file for writing: %s", g_LogFilePath.c_str());

    // Try alternative path
    std::string altPath = "/data/data/com.dts.freefiremax/zygisk_ff_logs.txt";
    LOGW("Trying alternative path: %s", altPath.c_str());

    logFile.open(altPath, std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
      LOGE("Failed to open alternative log file path");
      return;
    }
    g_LogFilePath = altPath;  // Update path if successful
    LOGI("Successfully switched to alternative log path");
  }

  try {
    logFile << "=== ZYGISK FF COMPLETE LOG DUMP ===" << std::endl;
    logFile << "Generated at: " << GetCurrentTimestamp() << std::endl;
    logFile << "Total entries: " << g_LogBuffer.size() << std::endl;
    logFile << "====================================" << std::endl;

    size_t written = 0;
    for (const auto &entry : g_LogBuffer) {
      logFile << "[" << entry.timestamp << "] [" << entry.level << "] " << entry.message << std::endl;
      written++;

      // Flush every 100 entries to ensure data is written
      if (written % 100 == 0) {
        logFile.flush();
      }
    }

    logFile << "=== END OF LOG DUMP ===" << std::endl;
    logFile.flush();
    logFile.close();

    LOGI("Successfully saved %zu log entries to file", written);
  } catch (const std::exception &e) {
    LOGE("Exception occurred while writing logs: %s", e.what());
    logFile.close();
  }
}

void AddLog(const char *level, const char *fmt, ...) {
  char    buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  LogEntry entry;
  entry.timestamp = GetCurrentTimestamp();
  entry.level     = level;
  entry.message   = buffer;

  // Set color based on log level
  if (strcmp(level, "ERROR") == 0) {
    entry.color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);  // Red
  } else if (strcmp(level, "WARN") == 0) {
    entry.color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);  // Yellow
  } else if (strcmp(level, "INFO") == 0) {
    entry.color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);  // Green
  } else {
    entry.color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);  // Gray
  }

  g_LogBuffer.push_back(entry);

  // Write to file immediately if auto-save is enabled
  WriteLogToFile(entry.timestamp, entry.level, entry.message);

  // Keep buffer size under limit
  if (g_LogBuffer.size() > MAX_LOG_ENTRIES) {
    g_LogBuffer.erase(g_LogBuffer.begin());
  }
}

void RenderLogsWindow() {
  if (!g_LogsVisible) return;

  ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Debug Logs", &g_LogsVisible)) {
    // Controls
    if (ImGui::Button("Clear")) {
      g_LogBuffer.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save to File")) {
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

    // Log content
    if (ImGui::BeginChild("LogContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
      for (const auto &entry : g_LogBuffer) {
        ImGui::TextColored(entry.color, "[%s] [%s] %s", entry.timestamp.c_str(), entry.level.c_str(),
                           entry.message.c_str());
      }

      // Auto-scroll to bottom
      if (g_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
      }
    }
    ImGui::EndChild();
  }
  ImGui::End();
}
