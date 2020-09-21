#include "startup/scan_group_common.h"

#include "utils/module_info.h"

namespace startup {
std::unique_ptr<ScanGroup> GetScanGroupTits(const std::vector<utils::SectionInfo>&) {
    return std::make_unique<ScanGroupCommon>();
}
}  // namespace startup
