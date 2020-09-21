#include "player/decoder.h"

#include <fstream>

namespace {
using player::Decoder;
using std::int32_t;
using std::uint32_t;
using std::uint16_t;

constexpr uint32_t kTag_RIFF = 0x46464952;
constexpr uint32_t kTag_WAVE = 0x45564157;
constexpr uint32_t kTag_fmt = 0x20746D66;
constexpr uint32_t kTag_data = 0x61746164;

class Wav : public Decoder {
public:
    static constexpr char Attr[] = "wav";
    static Decoder* const wav;

    bool Open(const char* file_name) override;
    std::size_t Read(BuffByte* buff, std::size_t samples_count) override;
    void Close() override;

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

private:
    static bool ready_;

public:
    static bool Init();
    static bool Ready() {
        return ready_;
    }
};  // Wav

bool Wav::Open(const char* file_name) {
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
    if ((wave_format_.bits_per_sample + 7) / 8 != wave_format_.block_align
        || wave_format_.avg_bytes_per_sec != wave_format_.block_align * wave_format_.samples_per_sec) {
        ifs_.close();
        return false;
    }

    samples_total_ = head.size_data / wave_format_.block_align;
    samples_read_ = 0;
    return true;
}

std::size_t Wav::Read(BuffByte* buff, std::size_t samples_count) {
    if (!buff || !samples_count) {
        return 0;
    }
    std::fill_n(buff, samples_count * wave_format_.block_align, BuffByte{ 0 });

    std::size_t read = std::min(samples_total_ - samples_read_, samples_count);
    ifs_.read(reinterpret_cast<char*>(buff), read * static_cast<std::streamsize>(wave_format_.block_align));

    samples_read_ += read;
    return read;
}

void Wav::Close() {
    ifs_.close();
}

bool Wav::ready_;

bool Wav::Init() {
    ready_ = true;
    return ready_;
}

}  // namesapce

std::unique_ptr<Decoder> player::Decoder::GetWav() {
    return Wav::Ready() ? std::make_unique<Wav>() : nullptr;
}

bool player::Decoder::InitWav() {
    return Wav::Init();
}
