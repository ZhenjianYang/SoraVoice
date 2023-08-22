#include "player/decoders.h"

namespace player::impl {

extern const DecoderId DecoderOgg;
extern const DecoderId DecoderWav;
extern const DecoderId DecoderFfmpeg;

static const DecoderId _DecoerList[] = {
    DecoderOgg,
    DecoderWav,
    DecoderFfmpeg,
    {},
};
const DecoderId * const DecoerList = _DecoerList;

}  // namespace player::impl
