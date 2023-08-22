#include "player/decoders.h"

namespace player::impl {

extern DecoderId DecoderOgg;
extern DecoderId DecoderWav;

static const DecoderId* _DecoerList[] = {
    &DecoderOgg,
    &DecoderWav,
    nullptr
};
const DecoderId * const * const DecoerList = _DecoerList;

}  // namespace player::impl
