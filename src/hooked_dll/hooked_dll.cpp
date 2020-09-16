#include <Windows.h>

#ifdef DINPUT8
#define DLL_NAME dinput8
#define API_NAME DirectInput8Create
#define HOOKED_API Hooked_DirectInput8Create
#define PARAMS_DCL (HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
#define PARAMS_CALL (hinst, dwVersion, riidltf, ppvOut, punkOuter)
#define ERR_CODE 0x80070057
#elif defined(DSOUND)
#define DLL_NAME dsound
#define API_NAME DirectSoundCreate
#define HOOKED_API Hooked_DirectSoundCreate
#define PARAMS_DCL (LPGUID lpGuid, LPVOID* ppDS, LPUNKNOWN pUnkOuter)
#define PARAMS_CALL (lpGuid, ppDS, pUnkOuter)
#define ERR_CODE 0x88780032
#endif

#define _S(V) #V
#define S(V) _S(V)

#define NAME_OLD_DLL S(DLL_NAME) "_old.dll"
#define SYS_PATH_DLL "%systemroot%\\System32\\" S(DLL_NAME) ".dll"
#define STR_HOOKAPI_NAME S(API_NAME)

#define STR_ED_VOICE_DLL "voice/ed_voice.dll"
#define STR_STARTUP "StartUp"

#define MAX_PATH_LEN 512

typedef HRESULT WINAPI HookedAPIType PARAMS_DCL;
typedef int __cdecl EdVoiceAPIType();

#ifdef __cplusplus
extern "C"
#endif
HRESULT WINAPI HOOKED_API PARAMS_DCL {
#if _DEBUG
    MessageBox(0, "Pause", "Pause", 0);
#endif // _DEBUG

    static HookedAPIType* api_ori = NULL;
    static int tried = 0;

    if (!api_ori) {
        HMODULE dll = LoadLibraryA(NAME_OLD_DLL);
        if (!dll) {
            char buff[MAX_PATH_LEN + 1];
            ExpandEnvironmentStringsA(SYS_PATH_DLL, buff, sizeof(buff));
            dll = LoadLibraryA(buff);
        }
        if (dll) {
            api_ori = (HookedAPIType*)GetProcAddress(dll, STR_HOOKAPI_NAME);
            if (!api_ori) {
                FreeLibrary(dll);
            }
        }
    }
    HRESULT rst = api_ori ? api_ori PARAMS_CALL : (HRESULT)ERR_CODE;

    if (!tried) {
        tried = 1;
        HMODULE dll = LoadLibraryA(STR_ED_VOICE_DLL);
        if (dll) {
            EdVoiceAPIType* start_up = (EdVoiceAPIType*)GetProcAddress(dll, STR_STARTUP);
            if (!start_up || start_up()) {
                FreeLibrary(dll);
            }
        }
    }

    return rst;
}
