#include "load_ogg_apis.h"

#include <Windows.h>

namespace {
constexpr const char* kVorbisfileDlls[] = { "vorbisfile.dll" , "libvorbisfile.dll" };
constexpr const char* kDllDirs[] = { "dll/", "voice/" };
constexpr char kSTR_ov_open_callbacks[] = "ov_open_callbacks";
constexpr char kSTR_ov_info[] = "ov_info";
constexpr char kSTR_ov_read[] = "ov_read";
constexpr char kSTR_ov_clear[] = "ov_clear";
constexpr char kSTR_ov_pcm_total[] = "ov_pcm_total";
}  // namespace

bool utils::LoadOggApis(void** ov_open_callbacks, void** ov_info, void** ov_read,
                        void** ov_clear, void** ov_pcm_total) {
    HMODULE ogg_dll = NULL;
    for (const char* dll_dir : kDllDirs) {
        SetDllDirectoryA(dll_dir);
        for (const char* dll_name : kVorbisfileDlls) {
            ogg_dll = LoadLibraryA(dll_name);
            if (ogg_dll) {
                break;
            }
        }
    }
    SetDllDirectoryA(NULL);

    *ov_open_callbacks = nullptr;
    *ov_info = nullptr;
    *ov_read = nullptr;
    *ov_clear = nullptr;
    *ov_pcm_total = nullptr;
    if (ogg_dll) {
        *ov_open_callbacks = (void*)GetProcAddress(ogg_dll, kSTR_ov_open_callbacks);
        *ov_info = (void*)GetProcAddress(ogg_dll, kSTR_ov_info);
        *ov_read = (void*)GetProcAddress(ogg_dll, kSTR_ov_read);
        *ov_clear = (void*)GetProcAddress(ogg_dll, kSTR_ov_clear);
        *ov_pcm_total = (void*)GetProcAddress(ogg_dll, kSTR_ov_pcm_total);
    }
    return *ov_open_callbacks && *ov_info && *ov_read && *ov_clear && *ov_pcm_total;
}
