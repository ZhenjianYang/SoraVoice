#ifndef __UTILS_MODULE_INFO_H__
#define __UTILS_MODULE_INFO_H__

#include <vector>

#include "base/byte.h"
#include "utils/section_info.h"

namespace utils {

bool GetCurrentModuleInformation(byte* *base, int *size);

std::vector<SectionInfo> GetSectionsInfo(byte* base);

}  // namespace utils

#endif  // __UTILS_MODULE_INFO_H__
