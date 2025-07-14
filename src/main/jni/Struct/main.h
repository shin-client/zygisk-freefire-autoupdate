#pragma once

#include <map>
#include <string>

#include "Bools.h"
#include "tools.hpp"

inline std::map<std::string, void *> _methods;
inline std::map<std::string, size_t> _fields;

template <typename T>
inline T getFieldValue(void *instance, ulong offset) {
  return *(T *)((uintptr_t)instance + offset);
}

template <typename T>
inline T setFieldValue(void *instance, ulong offset, T value) {
  *(T *)((uintptr_t)instance + offset) = value;
}

void *getRealAddr(ulong offset);
