#include "player/decoders.h"

namespace player::impl {

extern const DecoderId DecoderOgg;
extern const DecoderId DecoderWav;
extern const DecoderId DecoderFfmpeg;

static const DecoderId _DecoderList[] = {
    DecoderOgg,
    DecoderWav,
    DecoderFfmpeg,
    {},
};
const DecoderId * const DecoderList = _DecoderList;
const DecoderId * const DefaultDecoder = &DecoderFfmpeg;

}  // namespace player::impl
