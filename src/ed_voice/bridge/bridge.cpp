#include "bridge.h"

#include "base/byte.h"
#include "global/global.h"
#include "core/sora_voice.h"

int __stdcall bridge::Play(void* b) {
    return global.sv->Play((byte*)b);
}
