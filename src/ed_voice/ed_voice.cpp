#include "ed_voice.h"

#include "startup/startup.h"
#include "utils/log.h"

namespace {
static_assert(sizeof(void*) == 4, "32 bits only!");
}  // namesapce

int __cdecl StartUp() {
    static int succeeded = 0;

    LOG("Start Up...");
    if (succeeded) {
        LOG("Already Started.");
        return 1;
    }

    succeeded = startup::StartUp();
    if (!succeeded) {
        LOG("Start Up Failed.");
        return succeeded;
    }

    LOG("Start Up Succeeded.");
    return succeeded;
}
