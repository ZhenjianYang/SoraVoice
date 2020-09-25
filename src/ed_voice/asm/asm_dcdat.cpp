#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_::dcdat() {
    __asm {
        push    ebx
        push    edi
        call    bridge::DecompressDat
        test    eax, eax
        je      next
        ret

        next :
        jmp      dword ptr[global.addrs.dcdat_next]
    }
}
