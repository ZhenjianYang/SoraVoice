#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_::textse() {
    __asm {
        cmp    dword ptr[global.sigs.no_textse], 0
        je     next
        jmp    dword ptr[global.addrs.textse_jmp]

        next :
        jmp    dword ptr[global.addrs.textse_next]
    }
}