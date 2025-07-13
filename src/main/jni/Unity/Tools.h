#include <jni.h>

#include <map>
#include <string>

#include "dobby/dobby.h"

namespace Tools {
void Hook(void *target, void *replace, void **backup);
bool Read(void *addr, void *buffer, size_t length);
bool Write(void *addr, void *buffer, size_t length);
bool ReadAddr(void *addr, void *buffer, size_t length);
bool WriteAddr(void *addr, void *buffer, size_t length);

bool PVM_ReadAddr(void *addr, void *buffer, size_t length);
bool PVM_WriteAddr(void *addr, void *buffer, size_t length);

bool IsPtrValid(void *addr);

uintptr_t GetBaseAddress(const char *name);
uintptr_t GetRealOffsets(const char *libraryName, uintptr_t relativeAddr);
uintptr_t GetEndAddress(const char *name);
uintptr_t FindPattern(const char *lib, const char *pattern);

std::string RandomString(const int len);
std::string GetPackageName(JNIEnv *env, jobject context);
const char *GetAndroidID(JNIEnv *env, jobject context);
const char *GetDeviceModel(JNIEnv *env);
const char *GetDeviceBrand(JNIEnv *env);
const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid);
std::string CalcMD5(std::string s);
}
