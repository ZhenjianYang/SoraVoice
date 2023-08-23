#ifndef __UTILS_MODULE_INFO_H__
#define __UTILS_MODULE_INFO_H__

#include <vector>

#include "utils/section_info.h"

namespace utils {

bool GetCurrentModuleInformation(uint8_t* *base, int *size);

std::vector<SectionInfo> GetSectionsInfo(uint8_t* base);

}  // namespace utils

#endif  // __UTILS_MODULE_INFO_H__
