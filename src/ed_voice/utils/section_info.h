#ifndef __UTILS_SECTION_INFO_H__
#define __UTILS_SECTION_INFO_H__

#include <string>

#include "base/byte.h"

namespace utils {
struct SectionInfo {
    std::string name;
    byte* start;
    byte* end;
    int size;
};
}  // namespace utils

#endif  // __UTILS_SECTION_INFO_H__
