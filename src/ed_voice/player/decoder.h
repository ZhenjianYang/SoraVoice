#ifndef __PLAYER_DECODER_H__
#define __PLAYER_DECODER_H__

#include <cstdint>
#include <memory>
#include <string_view>

namespace player {
class Decoder {
public:
    struct WaveFormat {
        std::uint16_t format_tag;        // format type
        std::uint16_t channels;          // number of channels (i.e. mono, stereo, etc.)
        std::uint32_t samples_per_sec;   // sample rate
        std::uint32_t avg_bytes_per_sec; // for buffer estimation
        std::uint16_t block_align;       // block size of data
        std::uint16_t bits_per_sample;   // number of bits per sample of mono data
    };
    using BuffByte = char;

    virtual bool Open(const char* file_name) = 0;
    virtual std::size_t Read(BuffByte* buff, std::size_t samples_count) = 0;
    virtual void Close() = 0;

    const WaveFormat& GetWaveFormat() {
        return wave_format_;
    }
    std::size_t SamplesTotal() const {
        return samples_total_;
    }
    std::size_t SamplesRead() const {
        return samples_read_;
    }
    virtual ~Decoder() = default;

protected:
    WaveFormat wave_format_ { };
    std::size_t samples_total_ = 0;
    std::size_t samples_read_ = 0;

public:
    static std::unique_ptr<Decoder> GetWav();
    static std::unique_ptr<Decoder> GetOgg();
    static bool InitAllDecoders() {
        return InitWav() && InitOgg();
    }

protected:
    static bool InitWav();
    static bool InitOgg();
};  // Decoder
}  // namespace player
#endif // __PLAYER_DECODER_H__
