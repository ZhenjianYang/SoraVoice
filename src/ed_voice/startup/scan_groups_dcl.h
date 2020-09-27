#ifndef __STARTUP_SCAN_GROUPS_DCLS_H__
#define __STARTUP_SCAN_GROUPS_DCLS_H__

#include <memory>
#include <vector>

#include "startup/scan_group.h"
#include "utils/module_info.h"

#define GET_SCAN_GROUP_FUN(GroupName_) GetScanGroup##GroupName_

#define DECLARE_SCAN_GROUP(GroupName_) \
    extern std::unique_ptr<startup::ScanGroup> GET_SCAN_GROUP_FUN(GroupName_)( \
            const std::vector<utils::SectionInfo>& section_info);

namespace startup {
DECLARE_SCAN_GROUP(Tits);
DECLARE_SCAN_GROUP(Za);
DECLARE_SCAN_GROUP(Sora);
}  // namespace startup

#define GET_SCAN_GROUP_FUN_LIST { \
    GET_SCAN_GROUP_FUN(Tits), \
    GET_SCAN_GROUP_FUN(Sora), \
    GET_SCAN_GROUP_FUN(Za), \
}

#endif  // __STARTUP_SCAN_GROUPS_DCLS_H__
