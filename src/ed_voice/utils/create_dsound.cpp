#include "create_dsound.h"

#include <Windows.h>
#include <dsound.h>

namespace {
constexpr char kSTR_dsound_dll[] = "dsound.dll";
constexpr char kSTR_DirectSoundCreate[] = "DirectSoundCreate8";
using CallDSCreate = decltype(DirectSoundCreate);
}  // namespace

bool utils::CreateDSound(void** ppDS, void* hwnd) {
    if (!ppDS || !hwnd) {
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
            if (DS_OK == pDS->SetCooperativeLevel(reinterpret_cast<HWND>(hwnd), DSSCL_PRIORITY)) {
                *ppDS = pDS;
                return true;
            }
            pDS->Release();
        }
    }
    FreeLibrary(dsound_dll);
    return false;
}
