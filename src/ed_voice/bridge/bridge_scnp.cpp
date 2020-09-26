#include "bridge.h"

#include <string.h>
#include <io.h>

namespace {
constexpr char kPathScena[] = "voice/scena";
constexpr char kAttr[] = ".bin";
constexpr char kPathScenaOld[] = "./data/scena";
}  // namespace

int __cdecl bridge::RedirectSceanPath(void*, char* buff, const char*, char* dir, const char* scn) {
    constexpr int len_old = sizeof(kPathScenaOld) - 1;
    constexpr int len_new = sizeof(kPathScena) - 1;
    static_assert(len_new <= len_old, "len_new > len_old");

    if (strcmp(dir, kPathScenaOld)) {
        return 0;
    }

    int i = 0;
    for (; i < len_new; i++) {
        buff[i] = kPathScena[i];
    }
    buff[i++] = '/';
    for (unsigned j = 0; scn[j]; j++, i++) {
        buff[i] = scn[j];
    }
    for (unsigned j = 0; j < sizeof(kAttr); j++, i++) {
        buff[i] = kAttr[j];
    }

    if (-1 == _access(buff, 4)) {
        return 0;
    }

    for (i = 0; i <= len_new; i++) {
        dir[i] = kPathScena[i];
    }
    return 1;
}
