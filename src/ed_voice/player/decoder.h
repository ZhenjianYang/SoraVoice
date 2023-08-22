#ifndef __PLAYER_DECODER_H__
#define __PLAYER_DECODER_H__

#include <cstdint>
#include <memory>
#include <string_view>

#include "player/player_base.h"

namespace player {
class Decoder {
public:
    virtual bool Open(const char* file_name) = 0;
    virtual int Read(BuffByte* buff, int samples_count) = 0;
    virtual void Close() = 0;

    const WaveFormat& GetWaveFormat() {
        return wave_format_;
    }
    int SamplesTotal() const {
        return samples_total_;
    }
    int SamplesRead() const {
        return samples_read_;
    }
    virtual ~Decoder() = default;

protected:
    WaveFormat wave_format_ { };
    int samples_total_ = 0;
    int samples_read_ = 0;

public:
    static std::unique_ptr<Decoder> Get(std::string_view decoder_name);
    static bool Init();
};  // Decoder
}  // namespace player
#endif  // __PLAYER_DECODER_H__
