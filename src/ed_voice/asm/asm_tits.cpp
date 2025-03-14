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
        push esi

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
        push    edx
        push    ecx
        call    bridge::LoadDatF
        test    eax, eax
        pop     ecx
        pop     edx
        je      short next
        ret

        next :
        jmp     dword ptr[global.addrs.ldat_next];
    }
}

__declspec(naked) void asm_tits::dcdat() {
    __asm {
        pushad
        push    edx
        push    ecx
        push    0
        call    bridge::DecompressDat
        add     esp, 12
        test    eax, eax
        popad
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
        pushad
        call   bridge::Stop
        popad

        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     short jump
        mov    dword ptr[global.sigs.no_dlgse], 0
        cmp    dword ptr[global.config.disable_dialog_se], 0
        je     short jump

        ret

        jump :
        jmp    dword ptr[global.addrs.dlgse_jmp]
    }
}

