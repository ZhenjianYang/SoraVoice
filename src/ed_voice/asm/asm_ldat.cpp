#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_::ldat() {
    __asm {
        call    bridge::LoadDat
        test    eax, eax
        je      next
        ret

        next :
        jmp    dword ptr[global.addrs.ldat_next];
    }
}