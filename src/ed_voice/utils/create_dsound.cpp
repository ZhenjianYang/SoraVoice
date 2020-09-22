#include "create_dsound.h"

#include <Windows.h>
#include <dsound.h>

namespace {
constexpr char kSTR_dsound_dll[] = "dsound.dll";
constexpr char kSTR_DirectSoundCreate[] = "DirectSoundCreate";
using CallDSCreate = decltype(DirectSoundCreate);
}  // namespace

bool utils::CreateDSound(void** ppDS) {
    if (!ppDS) {
        return false;
    }

    HWND hwnd = GetActiveWindow();
    if (!hwnd) {
        return false;
    }

    auto dsound_dll = LoadLibraryA(kSTR_dsound_dll);
    if (!dsound_dll) {
        return false;
    }

    auto pDirectSoundCreate = (CallDSCreate*)GetProcAddress(dsound_dll, kSTR_DirectSoundCreate);
    if (pDirectSoundCreate) {
        IDirectSound* pDS = NULL;
        if (DS_OK == pDirectSoundCreate(NULL, &pDS, NULL)) {
            if (DS_OK == pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)) {
                *ppDS = pDS;
                return true;
            }
            pDS->Release();
        }
    }
    FreeLibrary(dsound_dll);
    return false;
}
