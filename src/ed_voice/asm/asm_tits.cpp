#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_tits::text() {
    __asm {
        jne     short next
        pushad

        cmp     dword ptr[global.info.game], GameTitsFC
        je      short fc
        push    edi
        jmp     common

        fc:
        push ebx

        common:
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
        je      short next
        ret

        next :
        jmp     dword ptr[global.addrs.ldat_next];
    }
}

__declspec(naked) void asm_tits::dcdat() {
    __asm {
        push    edi
        push    ebx
        push    0
        call    bridge::DecompressDat
        add     esp, 12
        test    eax, eax
        je      short next
        ret

        next :
        jmp     dword ptr[global.addrs.dcdat_next]
    }
}

__declspec(naked) void asm_tits::textse() {
    __asm {
        cmp    dword ptr[global.sigs.no_textse], 0
        je     short next
        cmp    dword ptr[global.config.disable_text_se], 0
        je     short next
        jmp    dword ptr[global.addrs.textse_jmp]

        next :
        jmp    dword ptr[global.addrs.textse_next]
    }
}

__declspec(naked) void asm_tits::dlgse() {
    __asm {
        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     short next
        mov    dword ptr[global.sigs.no_dlgse], 0
        cmp    dword ptr[global.config.disable_dialog_se], 0
        je     short next

        pushad
        call   bridge::Stop
        popad
        jmp    dword ptr[global.addrs.dlgse_jmp]

        next :
        pushad
        call   bridge::Stop
        popad
        jmp    dword ptr[global.addrs.dlgse_next]
    }
}

