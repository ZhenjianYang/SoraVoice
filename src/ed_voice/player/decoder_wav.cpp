#include "player/decoder.h"
#include "player/decoders.h"

#include <fstream>

namespace {
constexpr uint32_t kTag_RIFF = 0x46464952;
constexpr uint32_t kTag_WAVE = 0x45564157;
constexpr uint32_t kTag_fmt = 0x20746D66;
constexpr uint32_t kTag_data = 0x61746164;
using player::Decoder;
using player::BuffByte;
using player::WaveFormat;

class Wav : public Decoder {
public:
    bool Open(const char* file_name) override {
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

    int Read(BuffByte* buff, int samples_count) override {
        if (!buff || !samples_count) {
            return 0;
        }
        std::fill_n(buff, samples_count * wave_format_.block_align, BuffByte{ 0 });
        if (samples_total_ <= samples_read_) {
            return 0;
        }

        int read = std::min(samples_total_ - samples_read_, samples_count);
        ifs_.read(reinterpret_cast<char*>(buff), read * static_cast<std::streamsize>(wave_format_.block_align));

        samples_read_ += read;
        return read;
    }

    void Close() override {
        ifs_.close();
    }

    Wav() = default;
    ~Wav() override {
        this->Close();
    }
private:
    std::ifstream ifs_;

    Wav(const Wav&) = delete;
    Wav& operator=(const Wav&) = delete;
    Wav(Wav&&) = delete;
    Wav& operator=(Wav&&) = delete;
};  // Wav

static bool Init() {
    return true;
}

static std::unique_ptr<player::Decoder> Get() {
    return std::make_unique<Wav>();
}

}// namesapce

namespace player::impl {
extern const DecoderId DecoderWav {
    "wav",
    &Init,
    &Get
};
}  // namespace player::impl
