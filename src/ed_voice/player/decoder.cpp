#include "player/decoder.h"

#include "player/decoder_ogg.h"
#include "player/decoder_wav.h"

bool player::Decoder::Init() {
	return player::impl::Ogg::Init() && player::impl::Wav::Init();
}

std::unique_ptr<player::Decoder> player::Decoder::Get(std::string_view decoder_name) {
	if (decoder_name == "ogg") {
		return player::impl::Ogg::Get();
	} else if (decoder_name == "wav") {
		return player::impl::Wav::Get();
	} else {
		return nullptr;
	}
}

