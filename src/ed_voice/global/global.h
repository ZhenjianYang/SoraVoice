#ifndef __GLOBAL_GLOBAL_H__
#define __GLOBAL_GLOBAL_H__

#include "global/addresses.h"
#include "global/info.h"
#include "global/signals.h"

struct SoraVoice;

typedef struct Global {
    Info info;
    Signals sigs;
    Addresses addrs;
    struct SoraVoice* sv;
} Global;

#ifdef __cplusplus
extern "C"
#endif // __cplusplus
extern Global global;

#endif  // __GLOBAL_GLOBAL_H__
