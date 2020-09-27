#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_sora::text() {
    __asm {
        jne     short jump

        pushad
        push edx
        call bridge::Play
        popad
        jmp dword ptr[global.addrs.text_next]

        jump:
        jmp dword ptr[global.addrs.text_jmp]
    }
}

__declspec(naked) void asm_sora::ldat() {
    __asm {
        call    bridge::LoadDat2
        test    eax, eax
        je      short next
        ret

        next :
        jmp     dword ptr[global.addrs.ldat_next];
    }
}

__declspec(naked) void asm_sora::dcdat() {
    __asm {
        call    bridge::DecompressDat
        test    eax, eax
        je      short next
        ret

        next :
        jmp     dword ptr[global.addrs.dcdat_next]
    }
}

__declspec(naked) void asm_sora::textse() {
    __asm {
        cmp    dword ptr[global.sigs.no_textse], 0
        je     short jump
        cmp    dword ptr[global.config.disable_text_se], 0
        je     short jump

        ret

        jump :
        jmp    dword ptr[global.addrs.textse_jmp]
    }
}

__declspec(naked) void asm_sora::dlgse() {
    __asm {
        call   bridge::Stop

        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     short jump
        mov    dword ptr[global.sigs.no_dlgse], 0
        cmp    dword ptr[global.config.disable_dialog_se], 0
        je     short jump

        ret

        jump:
        jmp    dword ptr[global.addrs.dlgse_jmp]
    }
}

