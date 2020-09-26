#ifndef __CORE_LOAD_CONFIG_H__
#define __CORE_LOAD_CONFIG_H__

#include "global/config.h"
#include "global/info.h"

namespace core {
// if failed, will reset config
bool LoadConfig(Config* config, const char* filename);
bool SaveConfig(const Config* config, const char* filename, const Info* info);
}  // namespace core

#endif  // __CORE_LOAD_CONFIG_H__
