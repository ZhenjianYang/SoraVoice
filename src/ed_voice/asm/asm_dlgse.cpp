#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_::dlgse() {
    __asm {
        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     next
        mov    dword ptr[global.sigs.no_dlgse], 0
        jmp    dword ptr[global.addrs.dlgse_jmp]

        next :
        jmp    dword ptr[global.addrs.dlgse_next]
    }
}