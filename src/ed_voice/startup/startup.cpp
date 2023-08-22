#include "startup.h"

#include "core/sora_voice.h"
#include "global/global.h"
#include "startup/scan_group.h"
#include "utils/log.h"

namespace startup {
bool StartUp() {
    LOG("Generated Scan Groups...");
    auto groups = startup::ScanGroup::GetScanGroups();
    LOG("%d groups generated.", groups.size());

    for (int i = 0; i < groups.size(); i++) {
        LOG("Group #%d,%s", i, groups[i]->Name().c_str());
        if (!groups[i]->IsValid()) {
            LOG("Not valid, skipped.");
            continue;
        }

        global = {};
        LOG("Group #%d,%s: Scanning...", i, groups[i]->Name().c_str());
        if (!groups[i]->Scan()) {
            LOG("Group #%d,%s: Scan Failed", i, groups[i]->Name().c_str());
            continue;
        }
        LOG("Group #%d,%s: Scan Succeeded!", i, groups[i]->Name().c_str());
        LOG("Game Title: %s", groups[i]->GameTitle().c_str());
        LOG("Game Built Date: %s", groups[i]->GameBuiltDate().c_str());

        LOG("Group #%d,%s: Injecting...", i, groups[i]->Name().c_str());
        if (!groups[i]->Inject()) {
            LOG("Group #%d,%s: Inject Failed", i, groups[i]->Name().c_str());
            break;
        }
        LOG("Group #%d,%s: Inject Succeeded!", i, groups[i]->Name().c_str());

        LOG("SoraVoice Starting...");
        auto sora_voice = SoraVoice::GetSoraVoice(groups[i]->GameTitle(),
                                                  groups[i]->GameBuiltDate(),
                                                  std::move(groups[i]->MovableStrings()));
        if (!sora_voice) {
            LOG("SoraVoice Start Failed!");
            return false;
        }
        global.sv = sora_voice.release();
        LOG("SoraVoice Started!");
        return true;
    }

    return false;
}
}  // namespace startup
