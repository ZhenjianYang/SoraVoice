#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_tits::text() {
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

__declspec(naked) void asm_tits::ldat() {
    __asm {
        call    bridge::LoadDat
        test    eax, eax
        je      next
        ret

        next :
        jmp    dword ptr[global.addrs.ldat_next];
    }
}

__declspec(naked) void asm_tits::dcdat() {
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

__declspec(naked) void asm_tits::textse() {
    __asm {
        cmp    dword ptr[global.sigs.no_textse], 0
        je     next
        jmp    dword ptr[global.addrs.textse_jmp]

        next :
        jmp    dword ptr[global.addrs.textse_next]
    }
}

__declspec(naked) void asm_tits::dlgse() {
    __asm {
        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     next
        mov    dword ptr[global.sigs.no_dlgse], 0
        jmp    dword ptr[global.addrs.dlgse_jmp]

        next :
        jmp    dword ptr[global.addrs.dlgse_next]
    }
}

