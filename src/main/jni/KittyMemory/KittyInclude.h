#pragma once

#include "KittyUtils.h"
#include "KittyMemory.h"
#include "MemoryPatch.h"
#include "KittyScanner.h"
#include "KittyArm64.h"
#include "KittyPtrValidator.h"

#ifdef __ANDROID__
using KittyMemory::ProcMap;
using KittyScanner::RegisterNativeFn;
using KittyScanner::ElfScanner;

#elif __APPLE__
#include "writeData.h"
using KittyMemory::seg_data_t;
using KittyMemory::MemoryFileInfo;
#endif