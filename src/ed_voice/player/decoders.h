#ifndef __PLAYER_DECODERS_H__
#define __PLAYER_DECODERS_H__

#include "player/decoder.h"
#include <memory>

#define DECODER_NAME_MAXLEN 7

namespace player::impl {

struct DecoderId {
    const char Name[DECODER_NAME_MAXLEN + 1];
    bool (* const Init)();
    std::unique_ptr<Decoder>(* const Get)();
};

extern const DecoderId* const* const DecoerList;

}  // namespace player::impl

#endif  // __PLAYER_DECODER_H__