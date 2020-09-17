#include "ed_voice.h"

#include <startup/startup.h>
#include <utils/log.h>

namespace {
enum StartUpErrorCode {
    kSucceeded = 0,
    kSearchFailed,
    kInjectFailed,
    kStartFailed
};
}  // namesapce

int __cdecl StartUp() {
    static bool succeeded = false;

    LOG("Start Up...");
    if (succeeded) {
        LOG("Already Started.");
        return kSucceeded;
    }

    auto startup = startup::StartUp::GetInstance();
    if (!(succeeded = startup->Search())) {
        LOG("Start Up Failed: Search Failed");
        return kSearchFailed;
    }

    if (!(succeeded = startup->Inject())) {
        LOG("Start Up Failed: Inject Failed");
        return kInjectFailed;
    }

    if (!(succeeded = startup->Start())) {
        LOG("Start Up Failed: Start Failed");
        return kStartFailed;
    }

    LOG("Start Up Succeeded.");
    return kSucceeded;
}
