#include "scan_group.h"

#include "startup/scan_groups_dcl.h"
#include "utils/log.h"
#include "utils/module_info.h"

namespace startup {
std::vector<std::unique_ptr<ScanGroup>> startup::ScanGroup::GetScanGroups() {
    std::vector<std::unique_ptr<ScanGroup>> groups;

    LOG("Get exe module information...");
    uint8_t* base;
    int size;
    if (!utils::GetCurrentModuleInformation(&base, &size)) {
        LOG("Get exe module information Failed");
        return groups;
    }
    LOG("Exe module Info: Base = 0x%08X, Size = 0x%06X", (unsigned)base, size);

    LOG("Get sections information...");
    auto secs = utils::GetSectionsInfo(base);
    LOG("%d section(s).", secs.size());
    for (int i = 0; i < secs.size(); i++) {
        LOG("\n"
            "    Section #%d\n"
            "        Name : %s\n"
            "        Start: 0x%08X\n"
            "        End  : 0x%08X\n"
            "        Size : 0x%06X",
            i, secs[i].name.c_str(), (unsigned)secs[i].start, (unsigned)secs[i].end, secs[i].size);
    }

    for (auto get_scan_group_fun : GET_SCAN_GROUP_FUN_LIST) {
        groups.push_back(get_scan_group_fun(secs));
    }

    return groups;
}
}  // namespace startup
