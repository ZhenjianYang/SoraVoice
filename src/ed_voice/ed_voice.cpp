#include "ed_voice.h"

#include "startup/startup.h"
#include "utils/log.h"

namespace {
static_assert(sizeof(void*) == 4, "32 bits only!");

enum StartUpErrorCode {
    kSucceeded = 0,
    kFailed = -1
};
}  // namesapce

int __cdecl StartUp() {
    static bool succeeded = false;

    LOG("Start Up...");
    if (succeeded) {
        LOG("Already Started.");
        return kSucceeded;
    }

    succeeded = startup::StartUp();
    if (!succeeded) {
        LOG("Start Up Failed.");
        return kFailed;
    }

    LOG("Start Up Succeeded.");
    return kSucceeded;
}
