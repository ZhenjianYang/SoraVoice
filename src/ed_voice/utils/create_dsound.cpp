#include "create_dsound.h"

#include <Windows.h>
#include <dsound.h>

namespace {
constexpr int kMaxPathLen = 512;
constexpr char kDllPath[] = R"(%systemroot%\System32\dsound.dll)";
constexpr char kSTR_DirectSoundCreate8[] = "DirectSoundCreate8";
using CallDSCreate = decltype(DirectSoundCreate8);
}  // namespace

void* utils::CreateDSound8(void* hwnd) {
    if (!hwnd) {
        return nullptr;
    }

    char buff[kMaxPathLen + 1];
    ExpandEnvironmentStringsA(kDllPath, buff, sizeof(buff));
    auto dsound_dll = LoadLibraryA(buff);
    if (!dsound_dll) {
        return nullptr;
    }

    auto pDirectSoundCreate8 = (CallDSCreate*)GetProcAddress(dsound_dll, kSTR_DirectSoundCreate8);
    if (pDirectSoundCreate8) {
        IDirectSound8* pDS8 = NULL;
        if (DS_OK == pDirectSoundCreate8(NULL, &pDS8, NULL)) {
            if (DS_OK == pDS8->SetCooperativeLevel(reinterpret_cast<HWND>(hwnd), DSSCL_PRIORITY)) {
                return pDS8;
            }
            pDS8->Release();
        }
    }
    FreeLibrary(dsound_dll);
    return nullptr;
}
