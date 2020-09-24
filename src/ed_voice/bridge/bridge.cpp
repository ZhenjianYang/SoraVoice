#include "bridge.h"

#include "global/global.h"
#include "core/sora_voice.h"

int __stdcall bridge::Play(byte* b) {
    return global.sv->Play(b);
}
