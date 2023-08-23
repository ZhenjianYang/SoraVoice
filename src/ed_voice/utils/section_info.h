#ifndef __UTILS_SECTION_INFO_H__
#define __UTILS_SECTION_INFO_H__

#include <string>

namespace utils {
struct SectionInfo {
    std::string name;
    uint8_t* start;
    uint8_t* end;
    uint32_t size;
};
}  // namespace utils

#endif  // __UTILS_SECTION_INFO_H__
