#include "player/decoder.h"

#include <stdio.h>
#include <vorbis/vorbisfile.h>

#include "utils/load_ogg_apis.h"
#include "utils/log.h"

namespace {
using player::Decoder;
using std::int32_t;
using std::uint32_t;
using std::uint16_t;

class Ogg : public Decoder {
public:
    bool Open(const char* file_name) override;
    std::size_t Read(BuffByte* buff, std::size_t samples_count) override;
    void Close() override;

    Ogg() = default;
    ~Ogg() override {
        this->Close();
    }

private:
    OggVorbis_File ov_file{};

    Ogg(const Ogg&) = delete;
    Ogg& operator=(const Ogg&) = delete;
    Ogg(Ogg&&) = delete;
    Ogg& operator=(Ogg&&) = delete;

private:
    static decltype(::ov_open_callbacks)* ov_open_callbacks;
    static decltype(::ov_info)* ov_info;
    static decltype(::ov_read)* ov_read;
    static decltype(::ov_clear)* ov_clear;
    static decltype(::ov_pcm_total)* ov_pcm_total;
    static bool ready_;

public:
    static bool Init();
    static bool Ready() {
        return ready_;
    }
};

bool Ogg::Open(const char* file_name) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        return false;
    }

    if (ov_open_callbacks(file, &ov_file, nullptr, 0, OV_CALLBACKS_DEFAULT) != 0) {
        fclose(file);
        return false;
    }

    vorbis_info* info = ov_info(&ov_file, -1);

    wave_format_.format_tag = 1;
    wave_format_.channels = static_cast<uint16_t>(info->channels);
    wave_format_.samples_per_sec = info->rate;
    wave_format_.bits_per_sample = 16;
    wave_format_.block_align = static_cast<uint16_t>(info->channels * 16 / 8);
    wave_format_.avg_bytes_per_sec = wave_format_.samples_per_sec * wave_format_.block_align;

    samples_read_ = 0;
    samples_total_ = static_cast<std::size_t>(ov_pcm_total(&ov_file, -1));

    return true;
}

std::size_t Ogg::Read(BuffByte* buff, std::size_t samples_count) {
    if (!buff || !samples_count) {
        return 0;
    }
    std::fill_n(buff, samples_count * wave_format_.block_align, BuffByte{ 0 });

    std::size_t read = std::min(samples_total_ - samples_read_, samples_count);
    int bytes = static_cast<int>(read * wave_format_.block_align);
    int bitstream = 0;
    ov_read(&ov_file, reinterpret_cast<char*>(buff), bytes, 0, 2, 1, &bitstream);

    samples_read_ += read;
    return read;
}

void Ogg::Close() {
    ov_clear(&ov_file);
    memset(&ov_file, 0, sizeof(ov_file));
}

decltype(::ov_open_callbacks)* Ogg::ov_open_callbacks;
decltype(::ov_info)*           Ogg::ov_info;
decltype(::ov_read)*           Ogg::ov_read;
decltype(::ov_clear)*          Ogg::ov_clear;
decltype(::ov_pcm_total)*      Ogg::ov_pcm_total;
bool Ogg::ready_;

bool Ogg::Init() {
    if (!ready_) {
        LOG("Loading ogg APIs...");
        ready_ = utils::LoadOggApis(reinterpret_cast<void**>(&ov_open_callbacks),
                                    reinterpret_cast<void**>(&ov_info),
                                    reinterpret_cast<void**>(&ov_read),
                                    reinterpret_cast<void**>(&ov_clear),
                                    reinterpret_cast<void**>(&ov_pcm_total));
        if (ready_) {
            LOG("Load ogg APIs Finished.");
        } else {
            LOG("Load ogg APIs Finished.");
        }
        LOG("Loaded ov_open_callbacks = 0x%08X", (unsigned)ov_open_callbacks);
        LOG("Loaded ov_info = 0x%08X", (unsigned)ov_info);
        LOG("Loaded ov_read = 0x%08X", (unsigned)ov_read);
        LOG("Loaded ov_clear = 0x%08X", (unsigned)ov_clear);
        LOG("Loaded ov_pcm_total = 0x%08X", (unsigned)ov_pcm_total);
    }
    return ready_;
}
}  // namesapce

std::unique_ptr<Decoder> player::Decoder::GetOgg() {
    return Ogg::Ready() ? std::make_unique<Ogg>() : nullptr;
}

bool player::Decoder::InitOgg() {
    return Ogg::Init();
}
