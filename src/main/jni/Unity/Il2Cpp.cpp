#include "Il2cpp.h"

#include <android/log.h>
#include <jni.h>
#include <unistd.h>

#include <cstring>
#include <map>

#include "xdl/xdl.h"

#define g_LogTag "NgocDev"

typedef unsigned short UTF16;
typedef wchar_t        UTF32;
typedef char           UTF8;

namespace {
const void *(*il2cpp_assembly_get_image)(const void *assembly);
void *(*il2cpp_domain_get)();
void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size);
const char *(*il2cpp_image_get_name)(void *image);
void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name);
void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name);
void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount);
size_t (*il2cpp_field_get_offset)(void *field);
void (*il2cpp_field_static_get_value)(void *field, void *value);
void (*il2cpp_field_static_set_value)(void *field, void *value);
void *(*il2cpp_array_new)(void *elementTypeInfo, size_t length);
uint16_t *(*il2cpp_string_chars)(void *str);
Il2CppString *(*il2cpp_string_new)(const char *str);
Il2CppString *(*il2cpp_string_new_utf16)(const wchar_t *str, int32_t length);
char *(*il2cpp_type_get_name)(void *type);
void *(*il2cpp_method_get_param)(void *method, uint32_t index);
void *(*il2cpp_class_get_methods)(void *klass, void **iter);
const char *(*il2cpp_method_get_name)(void *method);
void *(*il2cpp_object_new)(void *klass);
}

int is_surrogate(UTF16 uc) { return (uc - 0xd800u) < 2048u; }

int is_high_surrogate(UTF16 uc) { return (uc & 0xfffffc00) == 0xd800; }

int is_low_surrogate(UTF16 uc) { return (uc & 0xfffffc00) == 0xdc00; }

UTF32 surrogate_to_utf32(UTF16 high, UTF16 low) { return (high << 10) + low - 0x35fdc00; }

const char *utf16_to_utf8(const UTF16 *source, size_t len) {
  std::u16string                                                    s(source, source + len);
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(s).c_str();
}

const wchar_t *utf16_to_utf32(const UTF16 *source, size_t len) {
  auto output = new UTF32[len + 1];

  for (int i = 0; i < len; i++) {
    const UTF16 uc = source[i];
    if (!is_surrogate(uc)) {
      output[i] = uc;
    } else {
      if (is_high_surrogate(uc) && is_low_surrogate(source[i]))
        output[i] = surrogate_to_utf32(uc, source[i]);
      else
        output[i] = L'?';
    }
  }

  output[len] = L'\0';
  return output;
}

// =========================================================================== //
const char *Il2CppString::CString() { return utf16_to_utf8(&this->start_char, this->length); }

const wchar_t *Il2CppString::WCString() { return utf16_to_utf32(&this->start_char, this->length); }

Il2CppString *Il2CppString::Create(const char *s) { return il2cpp_string_new(s); }

Il2CppString *Il2CppString::Create(const wchar_t *s, int len) { return il2cpp_string_new_utf16(s, len); }

void *Il2CppGetImageByName(const char *image) {
  size_t size;
  void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
  for (int i = 0; i < size; ++i) {
    void       *img      = (void *)il2cpp_assembly_get_image(assemblies[i]);
    const char *img_name = il2cpp_image_get_name(img);
    if (strcmp(img_name, image) == 0) {
      return img;
    }
  }
  return 0;
}

void *Il2CppGetClassType(const char *image, const char *namespaze, const char *clazz) {
  static std::map<std::string, void *> cache;
  std::string                          s = image;
  s += namespaze;
  s += clazz;
  if (cache.count(s) > 0) return cache[s];
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return 0;
  }
  void *klass = il2cpp_class_from_name(img, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find class %s!", clazz);
    return 0;
  }
  cache[s] = klass;
  return klass;
}

void *Il2CppCreateClassInstance(const char *image, const char *namespaze, const char *clazz) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return 0;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find class %s!", clazz);
    return 0;
  }
  void *obj = il2cpp_object_new(klass);
  if (!obj) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't create object %s!", clazz);
    return 0;
  }
  return obj;
}

void *Il2CppCreateArray(const char *image, const char *namespaze, const char *clazz, size_t length) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return 0;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find class %s!", clazz);
    return 0;
  }
  return il2cpp_array_new(klass, length);
}

void Il2CppGetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name,
                               void *output) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s!", name);
    return;
  }
  void *field = il2cpp_class_get_field_from_name(klass, name);
  if (!field) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s in class %s!", name, clazz);
    return;
  }
  il2cpp_field_static_get_value(field, output);
}

void Il2CppSetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name,
                               void *value) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s!", name);
    return;
  }
  void *field = il2cpp_class_get_field_from_name(klass, name);
  if (!field) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s in class %s!", name, clazz);
    return;
  }
  il2cpp_field_static_set_value(field, value);
}

void *Il2CppGetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name,
                            int argsCount) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return 0;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find method %s!", name);
    return 0;
  }
  void **method = (void **)il2cpp_class_get_method_from_name(klass, name, argsCount);
  if (!method) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find method %s in class %s!", name, clazz);
    return 0;
  }
  __android_log_print(ANDROID_LOG_DEBUG, g_LogTag, "%s - [%s] %s::%s: %p", image, namespaze, clazz, name, *method);
  return *method;
}

void *Il2CppGetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, char **args,
                            int argsCount) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return 0;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find class %s for method %s!", clazz, name);
    return 0;
  }
  void  *iter   = 0;
  int    score  = 0;
  void **method = (void **)il2cpp_class_get_methods(klass, &iter);

  while (method) {
    const char *fname = il2cpp_method_get_name(method);
    if (strcmp(fname, name) == 0) {
      for (int i = 0; i < argsCount; i++) {
        void *arg = il2cpp_method_get_param(method, i);
        if (arg) {
          const char *tname = il2cpp_type_get_name(arg);
          if (strcmp(tname, args[i]) == 0) {
            score++;
          } else {
            __android_log_print(ANDROID_LOG_INFO, g_LogTag,
                                "Argument at index %d didn't matched requested argument!\n\tRequested: %s\n\tActual: "
                                "%s\nnSkipping function...",
                                i, args[i], tname);
            score = 0;
            goto skip;
          }
        }
      }
    }
  skip:
    if (score == argsCount) {
      __android_log_print(ANDROID_LOG_DEBUG, g_LogTag, "%s - [%s] %s::%s: %p", image, namespaze, clazz, name, *method);
      return *method;
    }
    method = (void **)il2cpp_class_get_methods(klass, &iter);
  }
  __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Cannot find function %s in class %s!", name, clazz);
  return 0;
}

size_t Il2CppGetFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return -1;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s!", name);
    return -1;
  }
  void *field = il2cpp_class_get_field_from_name(klass, name);
  if (!field) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s in class %s!", clazz, name);
    return -1;
  }
  auto result = il2cpp_field_get_offset(field);
  __android_log_print(ANDROID_LOG_DEBUG, g_LogTag, "%s - [%s] %s::%s: %p", image, namespaze, clazz, name,
                      (void *)result);
  return result;
}

size_t Il2CppGetStaticFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name) {
  void *img = Il2CppGetImageByName(image);
  if (!img) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find image %s!", image);
    return -1;
  }
  void *klass = Il2CppGetClassType(image, namespaze, clazz);
  if (!klass) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s!", name);
    return -1;
  }

  FieldInfo *field = (FieldInfo *)il2cpp_class_get_field_from_name(klass, name);
  if (!field) {
    __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "Can't find field %s in class %s!", clazz, name);
    return -1;
  }
  return (unsigned long)((uint64_t)field->parent->static_fields + field->offset);
}

bool Il2CppIsAssembliesLoaded() {
  size_t size;
  void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
  return size != 0 && assemblies != 0;
}

void Il2CppAttach(const char *name) {
  void *handle = xdl_open(name, 0);
  while (!handle) {
    handle = xdl_open(name, 0);
    sleep(1);
  }
  il2cpp_assembly_get_image    = (const void *(*)(const void *))xdl_sym(handle, "il2cpp_assembly_get_image", 0);
  il2cpp_domain_get            = (void *(*)())xdl_sym(handle, "il2cpp_domain_get", 0);
  il2cpp_domain_get_assemblies = (void **(*)(const void *, size_t *))xdl_sym(handle, "il2cpp_domain_get_assemblies", 0);
  il2cpp_image_get_name        = (const char *(*)(void *))xdl_sym(handle, "il2cpp_image_get_name", 0);
  il2cpp_class_from_name =
      (void *(*)(const void *, const char *, const char *))xdl_sym(handle, "il2cpp_class_from_name", 0);
  il2cpp_class_get_field_from_name =
      (void *(*)(void *, const char *))xdl_sym(handle, "il2cpp_class_get_field_from_name", 0);
  il2cpp_class_get_method_from_name =
      (void *(*)(void *, const char *, int))xdl_sym(handle, "il2cpp_class_get_method_from_name", 0);
  il2cpp_field_get_offset       = (size_t (*)(void *))xdl_sym(handle, "il2cpp_field_get_offset", 0);
  il2cpp_field_static_get_value = (void (*)(void *, void *))xdl_sym(handle, "il2cpp_field_static_get_value", 0);
  il2cpp_field_static_set_value = (void (*)(void *, void *))xdl_sym(handle, "il2cpp_field_static_set_value", 0);
  il2cpp_array_new              = (void *(*)(void *, size_t))xdl_sym(handle, "il2cpp_array_new", 0);
  il2cpp_string_chars           = (uint16_t *(*)(void *))xdl_sym(handle, "il2cpp_string_chars", 0);
  il2cpp_string_new             = (Il2CppString * (*)(const char *)) xdl_sym(handle, "il2cpp_string_new", 0);
  il2cpp_string_new_utf16  = (Il2CppString * (*)(const wchar_t *, int32_t)) xdl_sym(handle, "il2cpp_string_new", 0);
  il2cpp_type_get_name     = (char *(*)(void *))xdl_sym(handle, "il2cpp_type_get_name", 0);
  il2cpp_method_get_param  = (void *(*)(void *, uint32_t))xdl_sym(handle, "il2cpp_method_get_param", 0);
  il2cpp_class_get_methods = (void *(*)(void *, void **))xdl_sym(handle, "il2cpp_class_get_methods", 0);
  il2cpp_method_get_name   = (const char *(*)(void *))xdl_sym(handle, "il2cpp_method_get_name", 0);
  il2cpp_object_new        = (void *(*)(void *))xdl_sym(handle, "il2cpp_object_new", 0);
  xdl_close(handle);
}
