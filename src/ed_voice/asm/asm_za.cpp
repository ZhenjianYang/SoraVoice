#include "asm/asm.h"
#include "bridge/bridge.h"
#include "global/global.h"

__declspec(naked) void asm_za::text() {
    __asm {
        jne     short next
        pushad
        
        push eax
        call bridge::Play

        popad
        jmp dword ptr[global.addrs.text_next]

        next :
        jmp dword ptr[global.addrs.text_jmp]
    }
}

__declspec(naked) void asm_za::scnp() {
    __asm {
        call    bridge::RedirectSceanPath;
        jmp     dword ptr[global.addrs.scnp_jmp];
    }
}

__declspec(naked) void asm_za::textse() {
    __asm {
        cmp    dword ptr[global.sigs.no_textse], 0
        je     short next
        cmp    dword ptr[global.config.disable_text_se], 0
        je     short next
        mov    dword ptr[esp + 0x10], 0;

        next :
        jmp    dword ptr[global.addrs.textse_jmp]
    }
}

__declspec(naked) void asm_za::dlgse() {
    __asm {
        cmp    dword ptr[global.sigs.no_dlgse], 0
        je     short next
        mov    dword ptr[global.sigs.no_dlgse], 0
        cmp    dword ptr[global.config.disable_dialog_se], 0
        je     short next

        mov    dword ptr[esp + 0x10], 0;

        next :
        pushad
        call   bridge::Stop
        popad
        jmp    dword ptr[global.addrs.dlgse_jmp]
    }
}

