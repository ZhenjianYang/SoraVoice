#include "player/decoder_wav.h"


namespace {
constexpr uint32_t kTag_RIFF = 0x46464952;
constexpr uint32_t kTag_WAVE = 0x45564157;
constexpr uint32_t kTag_fmt = 0x20746D66;
constexpr uint32_t kTag_data = 0x61746164;
}// namesapce

bool player::impl::Wav::Open(const char* file_name) {
    ifs_ = std::ifstream(file_name, std::ios::binary | std::ios::in);
    if (!ifs_) {
        return false;
    }

    struct WavHead {
        uint32_t tag_RIFF;
        uint32_t size;
        uint32_t tag_WAVE;
        uint32_t tag_fmt;
        uint32_t size_wave_format;
        WaveFormat wave_format;
        uint32_t tag_data;
        uint32_t size_data;
    } head{ };

    if (!ifs_.read(reinterpret_cast<char*>(&head), sizeof(head))) {
        ifs_.close();
        return false;
    }
    if (head.tag_RIFF != kTag_RIFF || head.tag_WAVE != kTag_WAVE || head.tag_fmt != kTag_fmt
        || head.tag_data != kTag_data || head.size_wave_format != sizeof(WaveFormat)) {
        ifs_.close();
        return false;
    }
    wave_format_ = head.wave_format;

    samples_total_ = head.size_data / wave_format_.block_align;
    samples_read_ = 0;
    return true;
}

std::size_t player::impl::Wav::Read(BuffByte* buff, std::size_t samples_count) {
    if (!buff || !samples_count) {
        return 0;
    }
    std::fill_n(buff, samples_count * wave_format_.block_align, BuffByte{ 0 });
    if (samples_total_ <= samples_read_) {
        return 0;
    }

    std::size_t read = std::min(samples_total_ - samples_read_, samples_count);
    ifs_.read(reinterpret_cast<char*>(buff), read * static_cast<std::streamsize>(wave_format_.block_align));

    samples_read_ += read;
    return read;
}

void player::impl::Wav::Close() {
    ifs_.close();
}

bool player::impl::Wav::Init() {
    return true;
}

std::unique_ptr<player::Decoder> player::impl::Wav::Get() {
    return std::make_unique<Wav>();
}
