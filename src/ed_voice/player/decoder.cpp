#include "player/decoder.h"
#include "player/decoders.h"

bool player::Decoder::Init() {
    for (auto decoder_id = *player::impl::DecoerList; decoder_id; ++decoder_id) {
        decoder_id->Init();
    }
    return true;
}

std::unique_ptr<player::Decoder> player::Decoder::Get(std::string_view decoder_name) {
    for (auto decoder_id = *player::impl::DecoerList; decoder_id; ++decoder_id) {
        if (decoder_id->Name == decoder_name) {
            auto decoder = decoder_id->Get();
            if (decoder) {
                return decoder;
            }
        }
    }
    for (auto decoder_id = *player::impl::DecoerList; decoder_id; ++decoder_id) {
        auto decoder = decoder_id->Get();
        if (decoder) {
            return decoder;
        }
    }
    return nullptr;
}

