#ifndef __UTILS_MODULE_INFO_H__
#define __UTILS_MODULE_INFO_H__

#include <string>
#include <vector>

#include "base/byte.h"

namespace utils {
struct SectionInfo {
    std::string name;
    byte* start;
    byte* end;
    std::size_t size;
};

bool GetCurrentModuleInformation(byte* *base, std::size_t *size);

std::vector<SectionInfo> GetSections(byte* base);

}  // namespace utils

#endif // __UTILS_MODULE_INFO_H__
