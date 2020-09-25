#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_::text() {
    __asm {
        jne short next
        pushad
        push ebx
        call bridge::Play
        popad
        jmp dword ptr[global.addrs.text_next]

        next :
        jmp dword ptr[global.addrs.text_jmp]
    }
}
