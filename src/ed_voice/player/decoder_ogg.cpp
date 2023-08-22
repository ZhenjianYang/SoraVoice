#include "player/decoder.h"
#include "player/decoders.h"

#include <stdio.h>
#include <memory>
#include <vorbis/vorbisfile.h>

#include "utils/load_ogg_apis.h"
#include "utils/log.h"

namespace {
using std::int32_t;
using std::uint32_t;
using std::uint16_t;
using player::Decoder;
using player::BuffByte;

decltype(::ov_open_callbacks)* _ov_open_callbacks;
decltype(::ov_info)* _ov_info;
decltype(::ov_read)* _ov_read;
decltype(::ov_clear)* _ov_clear;
decltype(::ov_pcm_total)* _ov_pcm_total;
bool _ready;

class Ogg : public Decoder {
public:
    bool Open(const char* file_name) override {
        FILE* file = fopen(file_name, "rb");
        if (file == NULL) {
            return false;
        }

        if (_ov_open_callbacks(file, &this->ov_file, nullptr, 0, OV_CALLBACKS_DEFAULT) != 0) {
            fclose(file);
            return false;
        }

        vorbis_info* info = _ov_info(&this->ov_file, -1);

        wave_format_.format_tag = 1;
        wave_format_.channels = static_cast<uint16_t>(info->channels);
        wave_format_.samples_per_sec = info->rate;
        wave_format_.bits_per_sample = 16;
        wave_format_.block_align = static_cast<uint16_t>(info->channels * 16 / 8);
        wave_format_.avg_bytes_per_sec = wave_format_.samples_per_sec * wave_format_.block_align;

        samples_read_ = 0;
        samples_total_ = static_cast<std::size_t>(_ov_pcm_total(&this->ov_file, -1));

        return true;
    }

    std::size_t Read(BuffByte* buff, std::size_t samples_count) override {
        if (!buff || !samples_count) {
            return 0;
        }
        std::fill_n(buff, samples_count * wave_format_.block_align, BuffByte{ 0 });
        if (samples_total_ <= samples_read_) {
            return 0;
        }

        std::size_t request = std::min(samples_total_ - samples_read_, samples_count);
        constexpr int block = 4096;
        std::size_t read = 0;
        int bitstream = 0;
        while (read < request) {
            int block_bytes = std::min(static_cast<int>(request - read) * wave_format_.block_align, block);
            int bytes_read = _ov_read(&this->ov_file, reinterpret_cast<char*>(buff), block_bytes, 0, 2, 1, &bitstream);
            read += bytes_read / wave_format_.block_align;
            buff += bytes_read;
        }
        samples_read_ += read;
        return read;
    }

    void Close() override {
        _ov_clear(&this->ov_file);
        memset(&this->ov_file, 0, sizeof(this->ov_file));
    }

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
};  // Ogg


static bool Init() {
    if (!_ready) {
        LOG("Loading ogg APIs...");
        _ready = utils::LoadOggApis(reinterpret_cast<void**>(&_ov_open_callbacks),
            reinterpret_cast<void**>(&_ov_info),
            reinterpret_cast<void**>(&_ov_read),
            reinterpret_cast<void**>(&_ov_clear),
            reinterpret_cast<void**>(&_ov_pcm_total));
        if (_ready) {
            LOG("Load ogg APIs Finished.");
        }
        else {
            LOG("Load ogg APIs Failed.");
        }
        LOG("Loaded _ov_open_callbacks = 0x%08X", (unsigned)_ov_open_callbacks);
        LOG("Loaded _ov_info = 0x%08X", (unsigned)_ov_info);
        LOG("Loaded _ov_read = 0x%08X", (unsigned)_ov_read);
        LOG("Loaded _ov_clear = 0x%08X", (unsigned)_ov_clear);
        LOG("Loaded _ov_pcm_total = 0x%08X", (unsigned)_ov_pcm_total);
    }
    return _ready;
}

static std::unique_ptr<Decoder> Get() {
    return _ready ? std::make_unique<Ogg>() : nullptr;
}

} // namespace

namespace player::impl {
extern const DecoderId DecoderOgg {
    "ogg",
    &Init,
    &Get
};
}  // namespace player::impl
