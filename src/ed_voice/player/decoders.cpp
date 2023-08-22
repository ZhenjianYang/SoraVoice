#include "player/decoders.h"

namespace player::impl {

extern const DecoderId DecoderOgg;
extern const DecoderId DecoderWav;

static const DecoderId _DecoerList[] = {
    DecoderOgg,
    DecoderWav,
    {},
};
const DecoderId * const DecoerList = _DecoerList;

}  // namespace player::impl
